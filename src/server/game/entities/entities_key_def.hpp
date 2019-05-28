#pragma once


#include <string>


namespace zq {



struct ObjectKeys
{
	static constexpr const char* guid = "guid";	  //uint64
};

struct PlayerKeys : public ObjectKeys
{
	// 创角需要的key
	static constexpr const char* account_name = "account_name";	  // string
	static constexpr const char* name = "name";	 //string
	static constexpr const char* race = "race";	   // uint8
	static constexpr const char* player_class = "player_class";	// uint8
	static constexpr const char* gender = "gender";	   // uint8
	static constexpr const char* skin = "skin";		// uint8
	static constexpr const char* face = "face";	   // uint8
	static constexpr const char* hair_style = "hair_style";	   // uint8
	static constexpr const char* hari_color = "hari_color";	  // uint8
	static constexpr const char* facial_style = "facial_style";	 // uint8
	static constexpr const char* facial_hair = "facial_hair";	 // uint8
	static constexpr const char* outfitid = "outfitid";	 // uint8
	static constexpr const char* lv = "lv";		   // uint8
	static constexpr const char* zone = "zone";		// int32
	static constexpr const char* mapid = "mapid";	   // int32
	static constexpr const char* pos_x = "pos_x";	// float
	static constexpr const char* pos_y = "pos_y";	 // float
	static constexpr const char* pos_z = "pos_z";	  // float
	static constexpr const char* orientation = "orientation";	  // float
	static constexpr const char* transport_id = "transport_id";	  // int32
	static constexpr const char* pet_display_id = "pet_display_id";	 // int32
	static constexpr const char* pet_lv = "pet_lv";				// int32
	static constexpr const char* pet_family = "pet_family";	   // int32

	// 进入游戏的key
	static constexpr const char* health = "health";	   // int32
	static constexpr const char* mana = "mana";	   // int32
	static constexpr const char* energy = "energy";	   // int32
	static constexpr const char* max_rage = "max_rage";	   // int32
	static constexpr const char* rage = "rage";	   // int32
	static constexpr const char* focus = "focus";	   // int32
	static constexpr const char* happiness = "happiness";	   // int32
	static constexpr const char* runic_power = "runic_power";	   // int32
	static constexpr const char* dungeon_difficulty = "dungeon_difficulty";	   // int32
};

struct ItemKeys : public ObjectKeys
{
	static constexpr const char* item_id = "item_id";	  // int32
	static constexpr const char* item_count = "item_count";	  // int32
};

}

