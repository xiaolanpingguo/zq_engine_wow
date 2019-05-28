#include "player.h"
#include "item.h"
#include "bag.h"
#include "entities_key_def.hpp"
#include "baselib/message/game_db_object.pb.h"


namespace zq {



Player::Player(WorldSession* session, const std::string& guid)
	:Entity(guid),
	session_(session)
{
	equip_.fill(nullptr);
}

Player::~Player()
{
	for (auto& ele : equip_)
	{
		if (ele)
		{
			delete ele;
		}
	}
}

bool Player::create(const PlayerCreateInfo& createInfo)
{
	//data_.appendValue(PlayerKeys::name, createInfo.Name);
	////data_.appendValue(PlayerKeys::guid, guid);
	//data_.appendValue(PlayerKeys::race, createInfo.Race);
	//data_.appendValue(PlayerKeys::player_class, createInfo.Class);
	//data_.appendValue(PlayerKeys::gender, createInfo.Gender);
	//data_.appendValue(PlayerKeys::skin, createInfo.Skin);
	//data_.appendValue(PlayerKeys::face, createInfo.Face);
	//data_.appendValue(PlayerKeys::hair_style, createInfo.HairStyle);
	//data_.appendValue(PlayerKeys::hari_color, createInfo.HairColor);
	//data_.appendValue(PlayerKeys::facial_hair, createInfo.FacialHair);
	//data_.appendValue(PlayerKeys::outfitid, createInfo.OutfitId);
	//data_.appendValue(PlayerKeys::lv, info->level);
	//data_.appendValue(PlayerKeys::zone, info->zone_id);
	//data_.appendValue(PlayerKeys::mapid, info->map_id);
	//data_.appendValue(PlayerKeys::pos_x, info->position_x);
	//data_.appendValue(PlayerKeys::pos_y, info->position_y);
	//data_.appendValue(PlayerKeys::pos_z, info->position_z);
	//data_.appendValue(PlayerKeys::transport_id, 0);

	//if (CharStartOutfitEntry const* oEntry = GetCharStartOutfitEntry(Race, Class, Gender); oEntry)
	//{
	//	for (int j = 0; j < MAX_OUTFIT_ITEMS; ++j)
	//	{
	//		uint32 itemId = oEntry->ItemId[j];
	//		if (configModule_->getCsvRow<CSVItemTemplate>(itemId))
	//		{
	//			//base_info->outfit[j] = itemId;
	//		}
	//	}
	//}
	return true;
}

bool Player::parseFromString(const std::string& data)
{
	PBObject::Object pb_obj;
	if (!pb_obj.ParseFromString(data))
	{
		LOG_ERROR << "parseFromString faild!!";
		return false;
	}

	// 基本属性
	covertPbObjToBaseObj(pb_obj);

	for (int i = 0; i < pb_obj.obj_list_size(); ++i)
	{
		const PBObject::Object& member = pb_obj.obj_list(i);
		if (equip_.size() <= (size_t)i)
		{
			LOG_ERROR << fmt::format("size overflow, item_size:{}, i: {}", equip_.size(), i);
			break;
		}

		equip_[i] = nullptr;
		if (member.has_value())
		{
			equip_[i] = new Item();
			equip_[i]->covertPbObjToBaseObj(member);
		}
	}

	return true;
}

bool Player::serialize(std::string& str)
{
	PBObject::Object pb_obj;

	// 基本属性
	covertBaseObjToPbObj(pb_obj);

	// 这个对象中又包含了其他对象
	for (const auto& item : equip_)
	{
		PBObject::Object* member = pb_obj.add_obj_list();
		member->set_has_value(false);
		if (item)
		{
			member->set_has_value(true);
			member->set_key(item->getGuid());
			item->covertBaseObjToPbObj(*member);
		}
	}

	return pb_obj.SerializeToString(&str);
}

}
