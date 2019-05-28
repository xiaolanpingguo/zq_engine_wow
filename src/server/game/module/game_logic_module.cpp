#include "game_logic_module.h"

#include "world_session.h"
#include "world_packet.h"

#include "baselib/message/game_db_account.pb.h"
#include "baselib/message/game_db_object.pb.h"
#include "baselib/message/pbobj_convert.hpp"
#include "baselib/base_code/random.h"
#include "baselib/kernel/config_module.h"

#include "dependencies/zlib/zlib.h"
#include "midware/cryptography/sha1.h"
#include "midware/cryptography/big_number.h"
#include "midware/cryptography/auth_crypt.h"

#include "entities/player.h"
#include "entities/item.h"
#include "common/game_time.h"
#include "dbc_stores//dbc_stores.h"

#include "config_header/cfg_playercreateinfo.hpp"
#include "config_header/cfg_item_template.hpp"

#include "entities/entities_key_def.hpp"


namespace zq {



GameLogicModule::GameLogicModule(ILibManager* p)
{
	libManager_ = p;
}

GameLogicModule::~GameLogicModule()
{

}

bool GameLogicModule::init()
{
	kernelModule_ = libManager_->findModule<IKernelModule>();
	classModule_ = libManager_->findModule<IClassModule>();
	redisModule_ = libManager_->findModule<IRedisModule>();
	configModule_ = libManager_->findModule<IConfigModule>();

	dataAgentModule_ = libManager_->findModule<IDataAgentModule>();
	objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	addonsModule_ = libManager_->findModule<IAddonsModule>();
	playerMgrModule_ = libManager_->findModule<IPlayerMgrModule>();

	if (!configModule_->create<CSVItemTemplate>("cfg_item_template.csv"))	return false;
	if (!configModule_->create<CSVPlayerCreateInfo>("cfg_playercreateinfo.csv")) return false;

	memset(playerCreateInfo_, 0, sizeof(playerCreateInfo_));
	const auto& all_row = configModule_->getCsvRowAll<CSVPlayerCreateInfo>();
	for (auto& ele : *all_row)
	{
		int race_id = ele.second.race_id;
		int class_id = ele.second.class_id;
		playerCreateInfo_[race_id][class_id] = &ele.second;
	}

	return true;
}

bool GameLogicModule::initEnd()
{
	using namespace std::placeholders;
	addMsgFun(CMSG_PING, std::bind(&GameLogicModule::handlePing, this, _1, _2));
	addMsgFun(CMSG_KEEP_ALIVE, std::bind(&GameLogicModule::handleKeepLive, this, _1, _2));
	addMsgFun(CMSG_AUTH_SESSION, std::bind(&GameLogicModule::handleAuthSession, this, _1, _2));
	addMsgFun(CMSG_READY_FOR_ACCOUNT_DATA_TIMES, std::bind(&GameLogicModule::handleReadyForAccountDataTimes, this, _1, _2));
	addMsgFun(CMSG_REALM_SPLIT, std::bind(&GameLogicModule::handleRealmSplitOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_ENUM, std::bind(&GameLogicModule::handleCharEnumOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_CREATE, std::bind(&GameLogicModule::handleCharCreateOpcode, this, _1, _2));
	addMsgFun(CMSG_CHAR_DELETE, std::bind(&GameLogicModule::handleCharDeleteOpcode, this, _1, _2));

	return true;
}

bool GameLogicModule::run()
{
	return true;
}

bool GameLogicModule::loadFromDb(Entity* object, const std::string& db_data)
{
	if (object == nullptr)
	{
		return false;
	}

	return object->parseFromString(db_data);
}

bool GameLogicModule::saveDb(const std::string& key, Entity* object)
{
	if (object == nullptr)
	{
		return false;
	}

	std::string db_data;
	if (object->serialize(db_data))
	{
		dataAgentModule_->setRedisHashData(key, object->getGuid(), db_data);
		return true;
	}

	return false;
}

bool GameLogicModule::addMsgFun(int32 msgid, MsgCallBackFun&& fun)
{
	return msgFuns_.insert(std::make_pair(msgid, std::move(fun))).second;
}

void GameLogicModule::call(int32 msgid, WorldSession* session, WorldPacket& recvPacket)
{
	auto it = msgFuns_.find(msgid);
	if (it != msgFuns_.end())
	{
		(it->second)(session, recvPacket);
	}
	else
	{
		LOG_ERROR << "can't find msgid, msgid:{}." << msgid;
	}
}

bool GameLogicModule::handleAuthSession(WorldSession* session, WorldPacket& recvPacket)
{
	if (session->isAuthed())
	{
		session->close();
		return false;
	}

	uint32 BattlegroupID = 0;
	uint32 LoginServerType = 0;
	uint32 RealmID = 0;
	uint32 Build = 0;
	uint32 LocalChallenge = 0;
	uint32 LoginServerID = 0;
	uint32 RegionID = 0;
	uint64 DosResponse = 0;
	uint8 Digest[SHA_DIGEST_LENGTH] = {};
	std::string Account;
	ByteBuffer AddonInfo;

	recvPacket >> Build;
	recvPacket >> LoginServerID;
	recvPacket >> Account;
	recvPacket >> LoginServerType;
	recvPacket >> LocalChallenge;
	recvPacket >> RegionID;
	recvPacket >> BattlegroupID;
	recvPacket >> RealmID;               // realmId from auth_database.realmlist table
	recvPacket >> DosResponse;
	recvPacket.read(Digest, sizeof(Digest));
	AddonInfo.append(recvPacket.contents() + recvPacket.rpos(), recvPacket.size() - recvPacket.rpos());

	// 去数据库查找帐号信息
	std::string hash_key = _KEY_ACCOUNT_ + Account;
	DBAccount::DBUserAccount account;
	if (!dataAgentModule_->getHashData(hash_key, Account, account))
	{
		sendAuthResponseError(session, AUTH_UNKNOWN_ACCOUNT);
		session->delayedClose();
		LOG_ERROR << "login faild, name: " << Account;
		return true;
	}

	// session key
	BigNumber session_key;
	session_key.SetHexStr(account.session_key().c_str());
	session->getAuthCrypt().Init(&session_key);

	uint32 t = 0;

	uint32 auth_seed = session->getAuthSeed();
	SHA1Hash sha;
	sha.UpdateData(Account);
	sha.UpdateData((uint8*)&t, 4);
	sha.UpdateData((uint8*)&LocalChallenge, 4);
	sha.UpdateData((uint8*)&auth_seed, 4);
	sha.UpdateBigNumbers(&session_key, nullptr);
	sha.Finalize();

	if (memcmp(sha.GetDigest(), Digest, SHA_DIGEST_LENGTH) != 0)
	{
		sendAuthResponseError(session, AUTH_FAILED);
		LOG_ERROR << fmt::format("Authentication failed for account: ('%s')", Account.c_str());
		session->delayedClose();
		return true;
	}

	int64 mutetime = 0;
	//int64 mutetime = account.MuteTime;
	////! Negative mutetime indicates amount of seconds to be muted effective on next login - which is now.
	//if (mutetime < 0)
	//{
	//	mutetime = GameTime::GetGameTime() + llabs(mutetime);

	//	stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME_LOGIN);
	//	stmt->setInt64(0, mutetime);
	//	stmt->setUInt32(1, account.Id);
	//	LoginDatabase.Execute(stmt);
	//}

	if (account.is_banned())
	{
		sendAuthResponseError(session, AUTH_BANNED);
		session->delayedClose();
		return true;
	}

	LOG_INFO << fmt::format("Client '%s' authenticated successfully.", Account.c_str());

	session->setAuthed(true);
	session->setAccoutName(Account);

	int32 Expansion = 2;
	static uint64 account_id = 0;
	int8 Locale = 4;
	int32 Recruiter = 0;
	bool IsRectuiter = false;
	session->setAuthed(auth_seed);

	// 读取附加数据
	readAddonsInfo(session, AddonInfo);
	session->initializeSession();

	account_id++;

	return true;
}

void GameLogicModule::readAddonsInfo(WorldSession* session, ByteBuffer& data)
{
	if (data.rpos() + 4 > data.size())
		return;

	uint32 size;
	data >> size;

	if (!size)
		return;

	if (size > 0xFFFFF)
	{
		LOG_ERROR << "AddOnInfo too big, size: " << size;
		return;
	}

	uLongf uSize = size;
	uint32 pos = data.rpos();

	ByteBuffer addonInfo;
	addonInfo.resize(size);

	if (uncompress(addonInfo.contents(), &uSize, data.contents() + pos, data.size() - pos) == Z_OK)
	{
		uint32 addonsCount;
		addonInfo >> addonsCount;

		for (uint32 i = 0; i < addonsCount; ++i)
		{
			std::string addonName;
			uint8 enabled;
			uint32 crc, unk1;

			// check next addon data format correctness
			if (addonInfo.rpos() + 1 > addonInfo.size())
				return;

			addonInfo >> addonName;
			addonInfo >> enabled >> crc >> unk1;

			AddonInfo addon(addonName, enabled, crc, 2, true);
			SavedAddon const* savedAddon = addonsModule_->getAddonInfo(addonName);
			if (savedAddon)
			{
				if (addon.CRC != savedAddon->CRC)
					LOG_WARN << fmt::format("Addon: %s: modified (CRC: 0x%x) - accountID:{})", addon.Name.c_str(), savedAddon->CRC, session->getAccountId());
			}
			else
			{
				addonsModule_->saveAddon(addon);
				LOG_WARN << fmt::format("Addon: %s: unknown (CRC: 0x%x) - accountId %d (storing addon name and checksum to database)", addon.Name.c_str(), addon.CRC, session->getAccountId());
			}

			// @todo Find out when to not use CRC/pubkey, and other possible states.
			session->addonesData(addon);
		}

		uint32 currentTime;
		addonInfo >> currentTime;
	}
	else
	{
		LOG_ERROR << "AddOn: Addon packet uncompress error!";
	}
}

bool GameLogicModule::handlePing(WorldSession* session, WorldPacket& recvPacket)
{
	using namespace std::chrono;

	uint32 ping;
	uint32 latency;

	// Get the ping packet content
	recvPacket >> ping;
	recvPacket >> latency;

	if (session->getLastPingTime() == steady_clock::time_point())
	{
		session->setLastPingTime(steady_clock::now());
	}
	else
	{
		steady_clock::time_point now = steady_clock::now();
		steady_clock::duration diff = now - session->getLastPingTime();
		session->setLastPingTime(now);
	}

	//if (_worldSession)
	//{
	//	_worldSession->SetLatency(latency);
	//	_worldSession->ResetClientTimeDelay();
	//}
	//else
	//{
	//	LOG_ERROR << fmt::format("peer sent CMSG_PING, but is not authenticated or got recently kicked, address = %s", getIp().c_str());
	//	return false;
	//}

	WorldPacket packet(SMSG_PONG, 4);
	packet << ping;
	session->sendPacket(packet);

	return true;
}

bool GameLogicModule::handleKeepLive(WorldSession* session, WorldPacket& recvPacket)
{
	return true;
}

bool GameLogicModule::handleReadyForAccountDataTimes(WorldSession* session, WorldPacket& recvPacket)
{
	static const int mask = 0x15;
	WorldPacket data(SMSG_ACCOUNT_DATA_TIMES, 4 + 1 + 4 + NUM_ACCOUNT_DATA_TYPES * 4);
	data << uint32(GameTime::GetGameTime());                             // Server time
	data << uint8(1);
	data << uint32(mask);                                   // type mask
	for (uint32 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
		if (mask & (1 << i))
			data << uint32(session->GetAccountData(AccountDataType(i))->Time);// also unix time

	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleRealmSplitOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	uint32 unk;
	std::string split_date = "01/01/01";
	recvPacket >> unk;

	WorldPacket data(SMSG_REALM_SPLIT, 4 + 4 + split_date.size() + 1);
	data << unk;
	data << uint32(0x00000000);                             // realm split state
	// split states:
	// 0x0 realm normal
	// 0x1 realm split
	// 0x2 realm split pending
	data << split_date;
	session->sendPacket(data);
	return true;
}

bool GameLogicModule::handleCharEnumOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	const std::string& account_name = session->getAccoutName();
	auto redis_client = redisModule_->getClientBySuitConsistent();
	if (redis_client == nullptr)
	{
		return false;
	}

	WorldPacket packet(SMSG_CHAR_ENUM, 100);                  // we guess size
	std::vector<string_pair> values;
	redis_client->HGETALL(_KEY_PLAYER_ + account_name, values);

	packet << (uint8)values.size();
	for (const auto& ele : values)
	{
		Player player(session);
		if (!loadFromDb(&player, ele.second))
		{
			LOG_ERROR << fmt::format("loadFromDb faild, account_name:{}, hash_key:{}.", account_name, ele.first);
			continue;
		}

		BaseObject*	player_obj = &player.getData();
		uint64 guid = 0;
		try{
			guid = std::stoull(player_obj->getKey());
		}catch (const std::exception& e)
		{
			LOG_ERROR << "get player guid faild, exception: " << e.what();
			continue;
		}

		packet << guid;
		packet << player_obj->getValueString(PlayerKeys::name);                       // name
		packet << uint8(player_obj->getValueInt32(PlayerKeys::race));                                // race
		packet << uint8(player_obj->getValueInt32(PlayerKeys::player_class));                               // class
		packet << uint8(player_obj->getValueInt32(PlayerKeys::gender));    // gender
		packet << uint8(player_obj->getValueInt32(PlayerKeys::skin));
		packet << uint8(player_obj->getValueInt32(PlayerKeys::face));
		packet << uint8(player_obj->getValueInt32(PlayerKeys::hair_style));
		packet << uint8(player_obj->getValueInt32(PlayerKeys::hari_color));
		packet << uint8(player_obj->getValueInt32(PlayerKeys::facial_style));
		packet << uint8(player_obj->getValueInt32(PlayerKeys::lv));                   // level
		packet << uint32(player_obj->getValueInt32(PlayerKeys::zone));                 // zone
		packet << uint32(player_obj->getValueInt32(PlayerKeys::mapid));                 // map
		packet << player_obj->getValueFloat(PlayerKeys::pos_x);
		packet << player_obj->getValueFloat(PlayerKeys::pos_y);
		packet << player_obj->getValueFloat(PlayerKeys::pos_z);
		packet << uint32(player_obj->getValueInt32(PlayerKeys::transport_id));                // transport guild id

		uint32 charFlags = 0;
		uint16 atLoginFlags = 0;
		//uint16 atLoginFlags = fields[18].GetUInt16();
		//uint32 playerFlags = fields[17].GetUInt32();
		//if (atLoginFlags & AT_LOGIN_RESURRECT)
		//	playerFlags &= ~PLAYER_FLAGS_GHOST;
		//if (playerFlags & PLAYER_FLAGS_HIDE_HELM)
		//	charFlags |= CHARACTER_FLAG_HIDE_HELM;
		//if (playerFlags & PLAYER_FLAGS_HIDE_CLOAK)
		//	charFlags |= CHARACTER_FLAG_HIDE_CLOAK;
		//if (playerFlags & PLAYER_FLAGS_GHOST)
		//	charFlags |= CHARACTER_FLAG_GHOST;
		//if (atLoginFlags & AT_LOGIN_RENAME)
		//	charFlags |= CHARACTER_FLAG_RENAME;
		//if (fields[23].GetUInt32())
		//	charFlags |= CHARACTER_FLAG_LOCKED_BY_BILLING;
		////if (sWorld->getBoolConfig(CONFIG_DECLINED_NAMES_USED))
		////{
		////	if (!fields[24].GetString().empty())
		////		charFlags |= CHARACTER_FLAG_DECLINED;
		////}
		//else
		//	charFlags |= CHARACTER_FLAG_DECLINED;

		packet << uint32(charFlags);                             // character flags

		// character customize flags
		//if (atLoginFlags & AT_LOGIN_CUSTOMIZE)
		//	packet << uint32(CHAR_CUSTOMIZE_FLAG_CUSTOMIZE);
		//else if (atLoginFlags & AT_LOGIN_CHANGE_FACTION)
		//	packet << uint32(CHAR_CUSTOMIZE_FLAG_FACTION);
		//else if (atLoginFlags & AT_LOGIN_CHANGE_RACE)
		//	packet << uint32(CHAR_CUSTOMIZE_FLAG_RACE);
		//else
		packet << uint32(CHAR_CUSTOMIZE_FLAG_NONE);

		// First login
		packet << uint8(atLoginFlags & AT_LOGIN_FIRST ? 1 : 0);

		// Pets info
		uint32 petDisplayId = 0;
		uint32 petLevel = 0;
		CreatureFamily petFamily = CREATURE_FAMILY_NONE;
		packet << uint32(player_obj->getValueInt32(PlayerKeys::pet_display_id));
		packet << uint32(player_obj->getValueInt32(PlayerKeys::pet_lv));
		packet << uint32(player_obj->getValueInt32(PlayerKeys::pet_family));

		//Tokenizer equipment(fields[22].GetString(), ' ');
		auto& equip_slots = player.getEquipSlots();
		for (uint8 slot = 0; slot < INVENTORY_SLOT_BAG_END; ++slot)
		{
			Item* item = equip_slots[slot];
			if (item == nullptr)
			{
				packet << uint32(0);
				packet << uint8(0);
				packet << uint32(0);
				continue;
			}

			uint32 itemId = item->GetEntry();
			const CSVItemTemplate* proto = configModule_->getCsvRow<CSVItemTemplate>(itemId);
			if (proto == nullptr)
			{
				packet << uint32(0);
				packet << uint8(0);
				packet << uint32(0);
				continue;
			}
			
			//SpellItemEnchantmentEntry const* enchant = nullptr;
			//uint32 enchants = GetUInt32ValueFromArray(equipment, visualBase + 1);
			//for (uint8 enchantSlot = PERM_ENCHANTMENT_SLOT; enchantSlot <= TEMP_ENCHANTMENT_SLOT; ++enchantSlot)
			//{
			//	// values stored in 2 uint16
			//	uint32 enchantId = 0x0000FFFF & (enchants >> enchantSlot * 16);
			//	if (!enchantId)
			//		continue;

			//	enchant = sSpellItemEnchantmentStore.LookupEntry(enchantId);
			//	if (enchant)
			//		break;
			//}

			packet << uint32(proto->DisplayInfoID);
			packet << uint8(proto->InventoryType);
			packet << uint32(0);   // 			//packet << uint32(enchant ? enchant->aura_id : 0);
		}
	}

	session->sendPacket(packet);

	return true;
}

bool GameLogicModule::handleCharCreateOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	static constexpr auto finiteAlways = [](float f) { return std::isfinite(f) ? f : 0.0f; };

	PlayerCreateInfo createInfo;
	recvPacket >> createInfo.Name
		>> createInfo.Race
		>> createInfo.Class
		>> createInfo.Gender
		>> createInfo.Skin
		>> createInfo.Face
		>> createInfo.HairStyle
		>> createInfo.HairColor
		>> createInfo.FacialHair
		>> createInfo.OutfitId;

	CSVPlayerCreateInfo const* info = getPlayerCreateInfo(createInfo.Race, createInfo.Class);
	if (!info)
	{
		LOG_ERROR << fmt::format("Player::Create: Possible hacking attempt: Account tried to create a character named {} with an invalid race/class pair ({}/{}) - refusing to do so.",
			session->getAccoutName(), createInfo.Race, createInfo.Class);
		return false;
	}

	uint8 resp_code = CHAR_CREATE_SUCCESS;
	do 
	{
		Player player(session, objectMgrModule_->createGuid(HighGuid::Player));
		player.create(createInfo);

		BaseObject& player_data = player.getData();
		player_data.appendValue(PlayerKeys::account_name, session->getAccoutName());
		player_data.appendValue(PlayerKeys::name, createInfo.Name);
		player_data.appendValue(PlayerKeys::race, createInfo.Race);
		player_data.appendValue(PlayerKeys::player_class, createInfo.Class);
		player_data.appendValue(PlayerKeys::gender, createInfo.Gender);
		player_data.appendValue(PlayerKeys::skin, createInfo.Skin);
		player_data.appendValue(PlayerKeys::face, createInfo.Face);
		player_data.appendValue(PlayerKeys::hair_style, createInfo.HairStyle);
		player_data.appendValue(PlayerKeys::hari_color, createInfo.HairColor);
		player_data.appendValue(PlayerKeys::facial_hair, createInfo.FacialHair);
		player_data.appendValue(PlayerKeys::outfitid, createInfo.OutfitId);
		player_data.appendValue(PlayerKeys::lv, info->level);
		player_data.appendValue(PlayerKeys::zone, info->zone_id);
		player_data.appendValue(PlayerKeys::mapid, info->map_id);
		player_data.appendValue(PlayerKeys::pos_x, info->position_x);
		player_data.appendValue(PlayerKeys::pos_y, info->position_y);
		player_data.appendValue(PlayerKeys::pos_z, info->position_z);
		player_data.appendValue(PlayerKeys::transport_id, 0);
		player_data.appendValue(PlayerKeys::dungeon_difficulty, (int)Difficulty::RAID_DIFFICULTY_25MAN_HEROIC);

		if (CharStartOutfitEntry const* oEntry = GetCharStartOutfitEntry(createInfo.Race, createInfo.Class, createInfo.Gender); oEntry)
		{
			for (int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
			{
				uint32 itemId = oEntry->ItemId[j];
				if (const CSVItemTemplate* iProto = configModule_->getCsvRow<CSVItemTemplate>(itemId); iProto)
				{
					uint32 count = iProto->BuyCount;
					if (iProto->Class == ITEM_CLASS_CONSUMABLE && iProto->SubClass == ITEM_SUBCLASS_FOOD)
					{
						switch (iProto->Spells[0].SpellCategory)
						{
						case SPELL_CATEGORY_FOOD:                                // food
							count = createInfo.Class == CLASS_DEATH_KNIGHT ? 10 : 4;
							break;
						case SPELL_CATEGORY_DRINK:                                // drink
							count = 2;
							break;
						}
						if (iProto->GetMaxStackSize() < count)
							count = iProto->GetMaxStackSize();
					}

					playerMgrModule_->storeNewItemInBestSlots(&player, itemId, count);
				}
			}
		}

		saveDb(_KEY_PLAYER_ + session->getAccoutName(), &player);

	} while (0);

	WorldPacket data(SMSG_CHAR_CREATE, 1);
	data << uint8(resp_code);
	session->sendPacket(data);

	return true;
}

bool GameLogicModule::handleCharDeleteOpcode(WorldSession* session, WorldPacket& recvPacket)
{
	return true;
}

bool GameLogicModule::handlePlayerLogin(WorldSession* session, WorldPacket& recvPacket)
{
	if (session->playerLoading())
	{
		LOG_ERROR << "login faild ,player is loading";
		return false;
	}

	session->playerLoading(true);
	uint64 playerGuid;
	recvPacket >> playerGuid;

	std::string db_data;
	if (!dataAgentModule_->getRedisHashData(_KEY_PLAYER_ + session->getAccoutName(), std::to_string(playerGuid), db_data))
	{
		LOG_ERROR << fmt::format("get data faild, account_name:{}, player_gui:{}.", session->getAccoutName(), playerGuid);
		return false;
	}

	Player* player = new Player(session);
	if (!loadFromDb(player, db_data))
	{
		LOG_ERROR << fmt::format("loadFromDb faild, account_name:{}, playergid:{}.", session->getAccoutName(), playerGuid);
		return false;
	}

	// 基础属性
	BaseObject& player_data = player->getData();
	//player_data.appendValue(PlayerKeys::health, 10000);
	//player_data.appendValue(PlayerKeys::mana, 10000);
	//player_data.appendValue(PlayerKeys::energy, 10000);
	//player_data.appendValue(PlayerKeys::max_rage, 10000);
	//player_data.appendValue(PlayerKeys::rage, 10000);
	//player_data.appendValue(PlayerKeys::focus, 10000);
	//player_data.appendValue(PlayerKeys::face, 10000);
	//player_data.appendValue(PlayerKeys::happiness, 10000);
	//player_data.appendValue(PlayerKeys::runic_power, 10000);

	sendDungeonDifficulty(player, false);

	// 登录返回
	WorldPacket data(SMSG_LOGIN_VERIFY_WORLD, 20);
	data << player_data.getValueInt32(PlayerKeys::mapid);
	data << player_data.getValueFloat(PlayerKeys::pos_x);
	data << player_data.getValueFloat(PlayerKeys::pos_y);
	data << player_data.getValueFloat(PlayerKeys::pos_z);
	data << player_data.getValueFloat(PlayerKeys::orientation);
	session->sendPacket(data);

	// load player specific part before send times
	//LoadAccountData(holder->GetPreparedResult(PLAYER_LOGIN_QUERY_LOAD_ACCOUNT_DATA), PER_CHARACTER_CACHE_MASK);

	// 账户时间
	session->sendAccountDataTimes(PER_CHARACTER_CACHE_MASK);

	// 系统状态
	data.Initialize(SMSG_FEATURE_SYSTEM_STATUS, 2);         // added in 2.2.0
	data << uint8(2);                                       // unknown value
	data << uint8(0);                                       // enable(1)/disable(0) voice chat interface in client
	session->sendPacket(data);

	// motd
	sendMotd(player);

	// 还不知道是啥
	data.Initialize(SMSG_LEARNED_DANCE_MOVES, 4 + 4);
	data << uint32(0);
	data << uint32(0);
	session->sendPacket(data);

	return true;
}

void GameLogicModule::sendDungeonDifficulty(Player* player, bool IsInGroup)
{
	BaseObject& player_data = player->getData();

	uint8 val = 0x00000001;
	WorldPacket data(MSG_SET_DUNGEON_DIFFICULTY, 12);
	data << player_data.getValueInt32(PlayerKeys::dungeon_difficulty);
	data << uint32(val);
	data << uint32(IsInGroup);
	player->getSession()->sendPacket(data);
}

void GameLogicModule::sendMotd(Player* player)
{
	static const std::array<const char*, 2> msg{ "hell zq" ,"you are luck" };

	WorldPacket data(SMSG_MOTD);                     // new in 2.0.1
	data << uint32(sizeof(msg) / sizeof(const char*)); // line count
	for (auto& ele : msg)
	{
		data << ele;
	}

	player->getSession()->sendPacket(data);
}

void GameLogicModule::sendAuthResponseError(WorldSession* session, uint8 code)
{
	WorldPacket packet(SMSG_AUTH_RESPONSE, 1);
	packet << uint8(code);

	session->sendPacket(packet);
}

void GameLogicModule::onWorldSessionInit(WorldSession* session)
{

}

const CSVPlayerCreateInfo* GameLogicModule::getPlayerCreateInfo(int race_id, int class_id)
{
	if (race_id >= MAX_RACES || race_id <= 0)
		return nullptr;
	if (class_id >= MAX_CLASSES || class_id <= 0)
		return nullptr;

	return playerCreateInfo_[race_id][class_id];
}

}

