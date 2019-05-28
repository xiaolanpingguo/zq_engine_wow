#pragma once


#include "entity.h"
#include "../common/common_defines.h"
#include "../server/world_session.h"
#include "baselib/base_code/base_object.hpp"
#include <array>

namespace zq{


#define RAID_DIFFICULTY_MASK_25MAN 1    // since 25man difficulties are 1 and 3, we can check them like that

#define MAX_DUNGEON_DIFFICULTY     3
#define MAX_RAID_DIFFICULTY        4
#define MAX_DIFFICULTY             4


// Íæ¼Ò×´Ì¬
enum PlayerFlags
{
	PLAYER_FLAGS_GROUP_LEADER = 0x00000001,
	PLAYER_FLAGS_AFK = 0x00000002,
	PLAYER_FLAGS_DND = 0x00000004,
	PLAYER_FLAGS_GM = 0x00000008,
	PLAYER_FLAGS_GHOST = 0x00000010,
	PLAYER_FLAGS_RESTING = 0x00000020,
	PLAYER_FLAGS_UNK6 = 0x00000040,
	PLAYER_FLAGS_UNK7 = 0x00000080,               // pre-3.0.3 PLAYER_FLAGS_FFA_PVP flag for FFA PVP state
	PLAYER_FLAGS_CONTESTED_PVP = 0x00000100,               // Player has been involved in a PvP combat and will be attacked by contested guards
	PLAYER_FLAGS_IN_PVP = 0x00000200,
	PLAYER_FLAGS_HIDE_HELM = 0x00000400,
	PLAYER_FLAGS_HIDE_CLOAK = 0x00000800,
	PLAYER_FLAGS_PLAYED_LONG_TIME = 0x00001000,               // played long time
	PLAYER_FLAGS_PLAYED_TOO_LONG = 0x00002000,               // played too long time
	PLAYER_FLAGS_IS_OUT_OF_BOUNDS = 0x00004000,
	PLAYER_FLAGS_DEVELOPER = 0x00008000,               // <Dev> prefix for something?
	PLAYER_FLAGS_UNK16 = 0x00010000,               // pre-3.0.3 PLAYER_FLAGS_SANCTUARY flag for player entered sanctuary
	PLAYER_FLAGS_TAXI_BENCHMARK = 0x00020000,               // taxi benchmark mode (on/off) (2.0.1)
	PLAYER_FLAGS_PVP_TIMER = 0x00040000,               // 3.0.2, pvp timer active (after you disable pvp manually)
	PLAYER_FLAGS_UBER = 0x00080000,
	PLAYER_FLAGS_UNK20 = 0x00100000,
	PLAYER_FLAGS_UNK21 = 0x00200000,
	PLAYER_FLAGS_COMMENTATOR2 = 0x00400000,
	PLAYER_ALLOW_ONLY_ABILITY = 0x00800000,                // used by bladestorm and killing spree, allowed only spells with SPELL_ATTR0_REQ_AMMO, SPELL_EFFECT_ATTACK, checked only for active player
	PLAYER_FLAGS_UNK24 = 0x01000000,                // disabled all melee ability on tab include autoattack
	PLAYER_FLAGS_NO_XP_GAIN = 0x02000000,
	PLAYER_FLAGS_UNK26 = 0x04000000,
	PLAYER_FLAGS_UNK27 = 0x08000000,
	PLAYER_FLAGS_UNK28 = 0x10000000,
	PLAYER_FLAGS_UNK29 = 0x20000000,
	PLAYER_FLAGS_UNK30 = 0x40000000,
	PLAYER_FLAGS_UNK31 = 0x80000000
};

// 2^n values
enum AtLoginFlags
{
	AT_LOGIN_NONE = 0x000,
	AT_LOGIN_RENAME = 0x001,
	AT_LOGIN_RESET_SPELLS = 0x002,
	AT_LOGIN_RESET_TALENTS = 0x004,
	AT_LOGIN_CUSTOMIZE = 0x008,
	AT_LOGIN_RESET_PET_TALENTS = 0x010,
	AT_LOGIN_FIRST = 0x020,
	AT_LOGIN_CHANGE_FACTION = 0x040,
	AT_LOGIN_CHANGE_RACE = 0x080,
	AT_LOGIN_RESURRECT = 0x100,
};

enum CharacterCustomizeFlags
{
	CHAR_CUSTOMIZE_FLAG_NONE = 0x00000000,
	CHAR_CUSTOMIZE_FLAG_CUSTOMIZE = 0x00000001,       // name, gender, etc...
	CHAR_CUSTOMIZE_FLAG_FACTION = 0x00010000,       // name, gender, faction, etc...
	CHAR_CUSTOMIZE_FLAG_RACE = 0x00100000        // name, gender, race, etc...
};


enum CreatureFamily
{
	CREATURE_FAMILY_NONE = 0,
	CREATURE_FAMILY_WOLF = 1,
	CREATURE_FAMILY_CAT = 2,
	CREATURE_FAMILY_SPIDER = 3,
	CREATURE_FAMILY_BEAR = 4,
	CREATURE_FAMILY_BOAR = 5,
	CREATURE_FAMILY_CROCOLISK = 6,
	CREATURE_FAMILY_CARRION_BIRD = 7,
	CREATURE_FAMILY_CRAB = 8,
	CREATURE_FAMILY_GORILLA = 9,
	CREATURE_FAMILY_HORSE_CUSTOM = 10,   // Does not exist in DBC but used for horse like beasts in DB
	CREATURE_FAMILY_RAPTOR = 11,
	CREATURE_FAMILY_TALLSTRIDER = 12,
	CREATURE_FAMILY_FELHUNTER = 15,
	CREATURE_FAMILY_VOIDWALKER = 16,
	CREATURE_FAMILY_SUCCUBUS = 17,
	CREATURE_FAMILY_DOOMGUARD = 19,
	CREATURE_FAMILY_SCORPID = 20,
	CREATURE_FAMILY_TURTLE = 21,
	CREATURE_FAMILY_IMP = 23,
	CREATURE_FAMILY_BAT = 24,
	CREATURE_FAMILY_HYENA = 25,
	CREATURE_FAMILY_BIRD_OF_PREY = 26,
	CREATURE_FAMILY_WIND_SERPENT = 27,
	CREATURE_FAMILY_REMOTE_CONTROL = 28,
	CREATURE_FAMILY_FELGUARD = 29,
	CREATURE_FAMILY_DRAGONHAWK = 30,
	CREATURE_FAMILY_RAVAGER = 31,
	CREATURE_FAMILY_WARP_STALKER = 32,
	CREATURE_FAMILY_SPOREBAT = 33,
	CREATURE_FAMILY_NETHER_RAY = 34,
	CREATURE_FAMILY_SERPENT = 35,
	CREATURE_FAMILY_MOTH = 37,
	CREATURE_FAMILY_CHIMAERA = 38,
	CREATURE_FAMILY_DEVILSAUR = 39,
	CREATURE_FAMILY_GHOUL = 40,
	CREATURE_FAMILY_SILITHID = 41,
	CREATURE_FAMILY_WORM = 42,
	CREATURE_FAMILY_RHINO = 43,
	CREATURE_FAMILY_WASP = 44,
	CREATURE_FAMILY_CORE_HOUND = 45,
	CREATURE_FAMILY_SPIRIT_BEAST = 46
};

enum CharacterFlags
{
	CHARACTER_FLAG_NONE = 0x00000000,
	CHARACTER_FLAG_UNK1 = 0x00000001,
	CHARACTER_FLAG_UNK2 = 0x00000002,
	CHARACTER_LOCKED_FOR_TRANSFER = 0x00000004,
	CHARACTER_FLAG_UNK4 = 0x00000008,
	CHARACTER_FLAG_UNK5 = 0x00000010,
	CHARACTER_FLAG_UNK6 = 0x00000020,
	CHARACTER_FLAG_UNK7 = 0x00000040,
	CHARACTER_FLAG_UNK8 = 0x00000080,
	CHARACTER_FLAG_UNK9 = 0x00000100,
	CHARACTER_FLAG_UNK10 = 0x00000200,
	CHARACTER_FLAG_HIDE_HELM = 0x00000400,
	CHARACTER_FLAG_HIDE_CLOAK = 0x00000800,
	CHARACTER_FLAG_UNK13 = 0x00001000,
	CHARACTER_FLAG_GHOST = 0x00002000,
	CHARACTER_FLAG_RENAME = 0x00004000,
	CHARACTER_FLAG_UNK16 = 0x00008000,
	CHARACTER_FLAG_UNK17 = 0x00010000,
	CHARACTER_FLAG_UNK18 = 0x00020000,
	CHARACTER_FLAG_UNK19 = 0x00040000,
	CHARACTER_FLAG_UNK20 = 0x00080000,
	CHARACTER_FLAG_UNK21 = 0x00100000,
	CHARACTER_FLAG_UNK22 = 0x00200000,
	CHARACTER_FLAG_UNK23 = 0x00400000,
	CHARACTER_FLAG_UNK24 = 0x00800000,
	CHARACTER_FLAG_LOCKED_BY_BILLING = 0x01000000,
	CHARACTER_FLAG_DECLINED = 0x02000000,
	CHARACTER_FLAG_UNK27 = 0x04000000,
	CHARACTER_FLAG_UNK28 = 0x08000000,
	CHARACTER_FLAG_UNK29 = 0x10000000,
	CHARACTER_FLAG_UNK30 = 0x20000000,
	CHARACTER_FLAG_UNK31 = 0x40000000,
	CHARACTER_FLAG_UNK32 = 0x80000000
};

enum Gender
{
	GENDER_MALE = 0,
	GENDER_FEMALE = 1,
	GENDER_NONE = 2
};


enum EnchantmentSlot : uint16
{
	PERM_ENCHANTMENT_SLOT = 0,
	TEMP_ENCHANTMENT_SLOT = 1,
	SOCK_ENCHANTMENT_SLOT = 2,
	SOCK_ENCHANTMENT_SLOT_2 = 3,
	SOCK_ENCHANTMENT_SLOT_3 = 4,
	BONUS_ENCHANTMENT_SLOT = 5,
	PRISMATIC_ENCHANTMENT_SLOT = 6,                    // added at apply special permanent enchantment
	MAX_INSPECTED_ENCHANTMENT_SLOT = 7,

	PROP_ENCHANTMENT_SLOT_0 = 7,                    // used with RandomSuffix
	PROP_ENCHANTMENT_SLOT_1 = 8,                    // used with RandomSuffix
	PROP_ENCHANTMENT_SLOT_2 = 9,                    // used with RandomSuffix and RandomProperty
	PROP_ENCHANTMENT_SLOT_3 = 10,                   // used with RandomProperty
	PROP_ENCHANTMENT_SLOT_4 = 11,                   // used with RandomProperty
	MAX_ENCHANTMENT_SLOT = 12
};


enum EquipmentSlots : uint8                                 // 19 slots
{
	EQUIPMENT_SLOT_START = 0,
	EQUIPMENT_SLOT_HEAD = 0,
	EQUIPMENT_SLOT_NECK = 1,
	EQUIPMENT_SLOT_SHOULDERS = 2,
	EQUIPMENT_SLOT_BODY = 3,
	EQUIPMENT_SLOT_CHEST = 4,
	EQUIPMENT_SLOT_WAIST = 5,
	EQUIPMENT_SLOT_LEGS = 6,
	EQUIPMENT_SLOT_FEET = 7,
	EQUIPMENT_SLOT_WRISTS = 8,
	EQUIPMENT_SLOT_HANDS = 9,
	EQUIPMENT_SLOT_FINGER1 = 10,
	EQUIPMENT_SLOT_FINGER2 = 11,
	EQUIPMENT_SLOT_TRINKET1 = 12,
	EQUIPMENT_SLOT_TRINKET2 = 13,
	EQUIPMENT_SLOT_BACK = 14,
	EQUIPMENT_SLOT_MAINHAND = 15,
	EQUIPMENT_SLOT_OFFHAND = 16,
	EQUIPMENT_SLOT_RANGED = 17,
	EQUIPMENT_SLOT_TABARD = 18,
	EQUIPMENT_SLOT_END = 19
};

enum InventorySlots : uint8                                 // 4 slots
{
	INVENTORY_SLOT_BAG_START = 19,
	INVENTORY_SLOT_BAG_END = 23
};

enum InventoryPackSlots : uint8                             // 16 slots
{
	INVENTORY_SLOT_ITEM_START = 23,
	INVENTORY_SLOT_ITEM_END = 39
};

enum BankItemSlots                                          // 28 slots
{
	BANK_SLOT_ITEM_START = 39,
	BANK_SLOT_ITEM_END = 67
};

enum BankBagSlots                                           // 7 slots
{
	BANK_SLOT_BAG_START = 67,
	BANK_SLOT_BAG_END = 74
};

enum BuyBackSlots                                           // 12 slots
{
	// stored in m_buybackitems
	BUYBACK_SLOT_START = 74,
	BUYBACK_SLOT_END = 86
};

enum KeyRingSlots : uint8                                   // 32 slots
{
	KEYRING_SLOT_START = 86,
	KEYRING_SLOT_END = 118
};

enum CurrencyTokenSlots                                     // 32 slots
{
	CURRENCYTOKEN_SLOT_START = 118,
	CURRENCYTOKEN_SLOT_END = 150
};


enum PlayerSlots
{
	// first slot for item stored (in any way in player m_items data)
	PLAYER_SLOT_START = 0,
	// last+1 slot for item stored (in any way in player m_items data)
	PLAYER_SLOT_END = 150,
	PLAYER_SLOTS_COUNT = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

#define INVENTORY_SLOT_BAG_0    255


struct PlayerCreateInfo
{
	std::string Name;
	uint8 Race = 0;
	uint8 Class = 0;
	uint8 Gender = GENDER_NONE;
	uint8 Skin = 0;
	uint8 Face = 0;
	uint8 HairStyle = 0;
	uint8 HairColor = 0;
	uint8 FacialHair = 0;
	uint8 OutfitId = 0;
};

struct ItemPosCount
{
	ItemPosCount(uint16 _pos, uint32 _count) : pos(_pos), count(_count) { }
	bool isContainedIn(std::vector<ItemPosCount> const& vec) const
	{
		for (auto itr = vec.begin(); itr != vec.end(); ++itr)
			if (itr->pos == pos)
				return true;
		return false;
	}
	uint16 pos;
	uint32 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

class Item;
class Player : public Entity
{
public:
    explicit Player(WorldSession* session, const std::string& guid = "");
    ~Player(); 

	bool create(const PlayerCreateInfo& createInfo);
	bool parseFromString(const std::string& data) override;
	bool serialize(std::string& data) override;

	std::array<Item*, PLAYER_SLOTS_COUNT>& getEquipSlots() { return equip_; }
	WorldSession* getSession() { return session_; }

private:

	std::array<Item*, PLAYER_SLOTS_COUNT> equip_;

	WorldSession* session_;
};

}
