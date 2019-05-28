#pragma once


#include "interface_header/logic/IMMapMgrModule.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include <unordered_map>
#include <optional>


namespace zq {



class MMapMgrModule : public IMMapMgrModule
{
public:
	MMapMgrModule(ILibManager* p) { libManager_ = p; }

	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool finalize() override;

	typedef std::unordered_map<uint32, dtTileRef> MMapTileSet;
	typedef std::unordered_map<uint32, dtNavMeshQuery*> NavMeshQuerySet;

	// dummy struct to hold map's mmap data
	struct MMapData
	{
		MMapData(dtNavMesh* mesh) : navMesh(mesh) { }
		~MMapData()
		{
			for (NavMeshQuerySet::iterator i = navMeshQueries.begin(); i != navMeshQueries.end(); ++i)
				dtFreeNavMeshQuery(i->second);

			if (navMesh)
				dtFreeNavMesh(navMesh);
		}

		// we have to use single dtNavMeshQuery for every instance, since those are not thread safe
		NavMeshQuerySet navMeshQueries;     // instanceId to query

		dtNavMesh* navMesh;
		MMapTileSet loadedTileRefs;        // maps [map grid coords] to [dtTile]
	};
	typedef std::unordered_map<uint32, MMapData*> MMapDataSet;

	void InitializeThreadUnsafe(const std::vector<uint32>& mapIds);
	bool loadMap(const std::string& basePath, uint32 mapId, int32 x, int32 y);
	bool unloadMap(uint32 mapId, int32 x, int32 y);
	bool unloadMap(uint32 mapId);
	bool unloadMapInstance(uint32 mapId, uint32 instanceId);

	// the returned [dtNavMeshQuery const*] is NOT threadsafe
	dtNavMeshQuery const* GetNavMeshQuery(uint32 mapId, uint32 instanceId);
	dtNavMesh const* GetNavMesh(uint32 mapId);

	uint32 getLoadedTilesCount() const { return loadedTiles; }
	uint32 getLoadedMapsCount() const { return uint32(loadedMMaps.size()); }

private:
	bool loadMapData(uint32 mapId);
	uint32 packTileID(int32 x, int32 y);
	MMapDataSet::const_iterator GetMMapData(uint32 mapId) const;

private:

	MMapDataSet loadedMMaps;
	uint32 loadedTiles;
	bool thread_safe_environment;

};

}
