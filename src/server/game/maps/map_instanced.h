#pragma once


#include "map.h"
#include "../dbc_stores/dbc_enums.h"

namespace zq{

class MapInstanced : public Map
{
	friend class MapManager;
public:
    typedef std::unordered_map< uint32, Map*> InstancedMaps;

	MapInstanced(uint32 id, time_t expiry);
	virtual ~MapInstanced() { }

	void Update(uint32 diff) override;
	void DelayedUpdate(uint32 diff) override;
	//void RelocationNotify();
	void UnloadAll() override;

	virtual void InitVisibilityDistance() override;

	Map* CreateInstanceForPlayer(uint32 mapId, Player* player, uint32 loginInstanceId = 0);
	Map* FindInstanceMap(uint32 instanceId) const
	{
		InstancedMaps::const_iterator i = m_InstancedMaps.find(instanceId);
		return(i == m_InstancedMaps.end() ? nullptr : i->second);
	}
	bool DestroyInstance(InstancedMaps::iterator &itr);

	InstancedMaps &GetInstancedMaps() { return m_InstancedMaps; }

private:
	InstanceMap* CreateInstance(uint32 InstanceId, InstanceSave* save, Difficulty difficulty);
	BattlegroundMap* CreateBattleground(uint32 InstanceId, Battleground* bg);

private:

	InstancedMaps m_InstancedMaps;

	uint16 GridMapReference[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];

};

}
