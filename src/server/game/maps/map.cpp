#include "map.h"
//#include "Battleground.h"
//#include "CellImpl.h"
//#include "DatabaseEnv.h"
//#include "DisableMgr.h"
//#include "DynamicTree.h"
//#include "GameObjectModel.h"
//#include "GameTime.h"
//#include "GridNotifiers.h"
//#include "GridNotifiersImpl.h"
//#include "GridStates.h"
//#include "Group.h"
//#include "InstanceScript.h"
//#include "MapInstanced.h"
//#include "MapManager.h"
//#include "MMapFactory.h"
//#include "MotionMaster.h"
//#include "ObjectAccessor.h"
//#include "ObjectGridLoader.h"
//#include "ObjectMgr.h"
//#include "Pet.h"
//#include "PoolMgr.h"
//#include "ScriptMgr.h"
//#include "Transport.h"
//#include "Vehicle.h"
//#include "VMapFactory.h"
//#include "World.h"
#include <unordered_set>
#include <vector>

namespace zq{

u_map_magic MapMagic        = { {'M','A','P','S'} };
u_map_magic MapVersionMagic = { {'v','1','.','8'} };
u_map_magic MapAreaMagic    = { {'A','R','E','A'} };
u_map_magic MapHeightMagic  = { {'M','H','G','T'} };
u_map_magic MapLiquidMagic  = { {'M','L','I','Q'} };

static uint16 const holetab_h[4] = { 0x1111, 0x2222, 0x4444, 0x8888 };
static uint16 const holetab_v[4] = { 0x000F, 0x00F0, 0x0F00, 0xF000 };

#define DEFAULT_GRID_EXPIRY     300
#define MAX_GRID_LOAD_TIME      50
//#define MAX_CREATURE_ATTACK_RADIUS  (45.0f * sWorld->getRate(RATE_CREATURE_AGGRO))
#define MAX_CREATURE_ATTACK_RADIUS  (45.0f)

#define DEFAULT_VISIBILITY_INSTANCE 170.0f                  // default visible distance in instances, 170 yards

//GridState* si_GridStates[MAX_GRID_STATE];



Map::Map(uint32 id, time_t expiry, uint32 InstanceId, uint8 SpawnMode, Map* _parent)
	:mapid_(id)
{

}

Map::~Map()
{

}

void Map::InitVisibilityDistance()
{
	m_VisibleDistance = DEFAULT_VISIBILITY_DISTANCE;
	m_VisibilityNotifyPeriod = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
}

void Map::UnloadAll()
{
	//// clear all delayed moves, useless anyway do this moves before map unload.
	//_creaturesToMove.clear();
	//_gameObjectsToMove.clear();

	//for (GridRefManager<NGridType>::iterator i = GridRefManager<NGridType>::begin(); i != GridRefManager<NGridType>::end();)
	//{
	//	NGridType &grid(*i->GetSource());
	//	++i;
	//	UnloadGrid(grid, true);       // deletes the grid and removes it from the GridRefManager
	//}

	//for (TransportsContainer::iterator itr = _transports.begin(); itr != _transports.end();)
	//{
	//	Transport* transport = *itr;
	//	++itr;

	//	RemoveFromMap<Transport>(transport, true);
	//}

	//for (auto& cellCorpsePair : _corpsesByCell)
	//{
	//	for (Corpse* corpse : cellCorpsePair.second)
	//	{
	//		corpse->RemoveFromWorld();
	//		corpse->ResetMap();
	//		delete corpse;
	//	}
	//}

	//_corpsesByCell.clear();
	//_corpsesByPlayer.clear();
	//_corpseBones.clear();
}

InstanceMap::InstanceMap(uint32 id, time_t expiry, uint32 InstanceId, uint8 SpawnMode, Map* _parent)
	: Map(id, expiry, InstanceId, SpawnMode, _parent),
	m_resetAfterUnload(false), m_unloadWhenEmpty(false),
	i_data(nullptr), i_script_id(0)
{
	//lets initialize visibility distance for dungeons
	InitVisibilityDistance();

	// the timer is started by default, and stopped when the first player joins
	// this make sure it gets unloaded if for some reason no player joins
	m_unloadTimer = 1;
}

InstanceMap::~InstanceMap()
{
	delete i_data;
	i_data = nullptr;
}

void InstanceMap::InitVisibilityDistance()
{
	m_VisibleDistance = DEFAULT_VISIBILITY_INSTANCE;
	m_VisibilityNotifyPeriod = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
}



}
