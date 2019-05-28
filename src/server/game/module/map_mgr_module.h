#pragma once


#include "../maps/map.h"
#include "../maps/map_instanced.h"
#include "../maps/map_updater.h"
#include "../grids/grid_defines.h"
#include "../entities/position.h"
#include <mutex>

namespace zq{

class Player;
class Transport;
struct TransportCreatureProto;
class MapManager
{
public:
    static MapManager* instance();

    Map* CreateBaseMap(uint32 mapId);
    Map* FindBaseNonInstanceMap(uint32 mapId) const;
    Map* CreateMap(uint32 mapId, Player* player, uint32 loginInstanceId=0);
	Map* CreateInstanceForPlayer(uint32 mapId, Player* player, uint32 loginInstanceId = 0);

private:

	Map* FindBaseMap(uint32 mapId) const
	{
		MapMapType::const_iterator iter = maps_.find(mapId);
		return (iter == maps_.end() ? nullptr : iter->second);
	}

private:
    typedef std::unordered_map<uint32, Map*> MapMapType;

    MapMapType maps_;
	uint32 gridCleanUpDelay_;
	IntervalTimer timer_;
};


#define sMapMgr MapManager::instance()

}
