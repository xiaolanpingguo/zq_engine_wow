#pragma once


#include "interface_header/base/platform.h"


namespace zq{


enum Races
{
	RACE_NONE = 0,
	RACE_HUMAN = 1,
	RACE_ORC = 2,
	RACE_DWARF = 3,
	RACE_NIGHTELF = 4,
	RACE_UNDEAD_PLAYER = 5,
	RACE_TAUREN = 6,
	RACE_GNOME = 7,
	RACE_TROLL = 8,
	//RACE_GOBLIN             = 9,
	RACE_BLOODELF = 10,
	RACE_DRAENEI = 11,
	//RACE_FEL_ORC            = 12,
	//RACE_NAGA               = 13,
	//RACE_BROKEN             = 14,
	//RACE_SKELETON           = 15,
	//RACE_VRYKUL             = 16,
	//RACE_TUSKARR            = 17,
	//RACE_FOREST_TROLL       = 18,
	//RACE_TAUNKA             = 19,
	//RACE_NORTHREND_SKELETON = 20,
	//RACE_ICE_TROLL          = 21
	MAX_RACES         
};

	// max+1 for player race

#define RACEMASK_ALL_PLAYABLE \
    ((1<<(RACE_HUMAN-1))   |(1<<(RACE_ORC-1))          |(1<<(RACE_DWARF-1))   | \
     (1<<(RACE_NIGHTELF-1))|(1<<(RACE_UNDEAD_PLAYER-1))|(1<<(RACE_TAUREN-1))  | \
     (1<<(RACE_GNOME-1))   |(1<<(RACE_TROLL-1))        |(1<<(RACE_BLOODELF-1))| \
     (1<<(RACE_DRAENEI-1)))

#define RACEMASK_ALLIANCE \
    ((1<<(RACE_HUMAN-1)) | (1<<(RACE_DWARF-1)) | (1<<(RACE_NIGHTELF-1)) | \
     (1<<(RACE_GNOME-1)) | (1<<(RACE_DRAENEI-1)))


// Class value is index in ChrClasses.dbc
enum Classes
{
	CLASS_NONE = 0,
	CLASS_WARRIOR = 1,
	CLASS_PALADIN = 2,
	CLASS_HUNTER = 3,
	CLASS_ROGUE = 4,
	CLASS_PRIEST = 5,
	CLASS_DEATH_KNIGHT = 6,
	CLASS_SHAMAN = 7,
	CLASS_MAGE = 8,
	CLASS_WARLOCK = 9,
	//CLASS_UNK           = 10,
	CLASS_DRUID = 11 ,
	MAX_CLASSES
};


#define CLASSMASK_ALL_PLAYABLE \
    ((1<<(CLASS_WARRIOR-1))|(1<<(CLASS_PALADIN-1))|(1<<(CLASS_HUNTER-1))| \
    (1<<(CLASS_ROGUE-1))  |(1<<(CLASS_PRIEST-1)) |(1<<(CLASS_SHAMAN-1))| \
    (1<<(CLASS_MAGE-1))   |(1<<(CLASS_WARLOCK-1))|(1<<(CLASS_DRUID-1)) | \
    (1<<(CLASS_DEATH_KNIGHT-1)))




#define MAX_CREATURE_SPELL_DATA_SLOT 4

}
