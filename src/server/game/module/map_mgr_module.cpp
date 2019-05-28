#include "map_mgr_module.h"
#include "../entities/player.h"
#include "../server/world_session.h"
#include "../server/opcodes.h"
#include "../server/world_packet.h"
#include "../dbc_stores/dbc_stores.h"
//#include "../instances/instance_save_mgr.h"
//#include "../globals/object_accessor.h"
//#include "../entities/transport/transport.h"
//#include "../grids/grid_defines.h"
//#include "../instances/instance_script.h"
////#include "World.h"
//#include "../entities/corpse/corpse.h"
//#include "../globals/object_mgr.h"
//#include "../groups/group.h"

namespace zq{

MapManager::MapManager()
{
	gridCleanUpDelay_ = 300000; // 300s
    timer_.SetInterval(100); // 0.1s
}

Map* MapManager::CreateBaseMap(uint32 id)
{
    Map* map = FindBaseMap(id);

    if (map == nullptr)
    {
        MapEntry const* entry = sMapStore.LookupEntry(id);
        ASSERT(entry);

		if (entry->Instanceable())
		{
			map = new MapInstanced(id, gridCleanUpDelay_);
		}
        else
        {
            map = new Map(id, gridCleanUpDelay_, 0, REGULAR_DIFFICULTY);
            //map->LoadRespawnTimes();
            //map->LoadCorpseData();
        }

		maps_[id] = map;
    }

    return map;
}

Map* MapManager::FindBaseNonInstanceMap(uint32 mapId) const
{
    Map* map = FindBaseMap(mapId);
    if (map && map->Instanceable())
        return nullptr;
    return map;
}

Map* MapManager::CreateMap(uint32 id, Player* player, uint32 loginInstanceId)
{
    Map* m = CreateBaseMap(id);

    if (m && m->Instanceable())
        m = ((MapInstanced*)m)->CreateInstanceForPlayer(id, player, loginInstanceId);

    return m;
}

Map* MapManager::CreateInstanceForPlayer(uint32 mapId, Player* player, uint32 loginInstanceId)
{
	if (GetId() != mapId || !player)
		return nullptr;

	Map* map = nullptr;
	uint32 newInstanceId = 0;                       // instanceId of the resulting map

	//if (IsBattlegroundOrArena())
	//{
	//	// instantiate or find existing bg map for player
	//	// the instance id is set in battlegroundid
	//	newInstanceId = player->GetBattlegroundId();
	//	if (!newInstanceId)
	//		return nullptr;

	//	map = sMapMgr->FindMap(mapId, newInstanceId);
	//	if (!map)
	//	{
	//		if (Battleground* bg = player->GetBattleground())
	//			map = CreateBattleground(newInstanceId, bg);
	//		else
	//		{
	//			player->TeleportToBGEntryPoint();
	//			return nullptr;
	//		}
	//	}
	//}
	//else
	{
		InstancePlayerBind* pBind = player->GetBoundInstance(GetId(), player->GetDifficulty(IsRaid()));
		InstanceSave* pSave = pBind ? pBind->save : nullptr;

		// priority:
		// 1. player's permanent bind
		// 2. player's current instance id if this is at login
		// 3. group's current bind
		// 4. player's current bind
		if (!pBind || !pBind->perm)
		{
			if (loginInstanceId) // if the player has a saved instance id on login, we either use this instance or relocate him out (return null)
			{
				map = FindInstanceMap(loginInstanceId);
				if (!map && pSave && pSave->GetInstanceId() == loginInstanceId)
					map = CreateInstance(loginInstanceId, pSave, pSave->GetDifficulty());
				return map;
			}

			InstanceGroupBind* groupBind = nullptr;
			Group* group = player->GetGroup();
			// use the player's difficulty setting (it may not be the same as the group's)
			if (group)
			{
				groupBind = group->GetBoundInstance(this);
				if (groupBind)
				{
					// solo saves should be reset when entering a group's instance
					player->UnbindInstance(GetId(), player->GetDifficulty(IsRaid()));
					pSave = groupBind->save;
				}
			}
		}
		if (pSave)
		{
			// solo/perm/group
			newInstanceId = pSave->GetInstanceId();
			map = FindInstanceMap(newInstanceId);
			// it is possible that the save exists but the map doesn't
			if (!map)
				map = CreateInstance(newInstanceId, pSave, pSave->GetDifficulty());
		}
		else
		{
			// if no instanceId via group members or instance saves is found
			// the instance will be created for the first time
			newInstanceId = sMapMgr->GenerateInstanceId();

			Difficulty diff = player->GetGroup() ? player->GetGroup()->GetDifficulty(IsRaid()) : player->GetDifficulty(IsRaid());
			//Seems it is now possible, but I do not know if it should be allowed
			//ASSERT(!FindInstanceMap(NewInstanceId));
			map = FindInstanceMap(newInstanceId);
			if (!map)
				map = CreateInstance(newInstanceId, nullptr, diff);
		}
	}

	return map;
}

Map* MapManager::FindMap(uint32 mapid, uint32 instanceId) const
{
    Map* map = FindBaseMap(mapid);
    if (!map)
        return nullptr;

    if (!map->Instanceable())
        return instanceId == 0 ? map : nullptr;

    return ((MapInstanced*)map)->FindInstanceMap(instanceId);
}

}
