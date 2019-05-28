#include "mmap_mgr_module.h"
#include "../collision/map_defines.h"

namespace zq {


static char const* const MAP_FILE_NAME_FORMAT = "%s/mmaps/%03i.mmap";
static char const* const TILE_FILE_NAME_FORMAT = "%s/mmaps/%03i%02i%02i.mmtile";


bool MMapMgrModule::init()
{
	loadedTiles = 0;
	thread_safe_environment = true;
	//objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	//configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool MMapMgrModule::initEnd()
{

	return true;
}

bool MMapMgrModule::run()
{

	return true;
}

bool MMapMgrModule::finalize()
{
	for (MMapDataSet::iterator i = loadedMMaps.begin(); i != loadedMMaps.end(); ++i)
		delete i->second;

	// by now we should not have maps loaded
	// if we had, tiles in MMapData->mmapLoadedTiles, their actual data is lost!
	return true;
}

void MMapMgrModule::InitializeThreadUnsafe(const std::vector<uint32>& mapIds)
{
	// the caller must pass the list of all mapIds that will be used in the VMapManager2 lifetime
	for (uint32 const& mapId : mapIds)
		loadedMMaps.insert(MMapDataSet::value_type(mapId, nullptr));

	thread_safe_environment = false;
}

MMapMgrModule::MMapDataSet::const_iterator MMapMgrModule::GetMMapData(uint32 mapId) const
{
	// return the iterator if found or end() if not found/NULL
	MMapDataSet::const_iterator itr = loadedMMaps.find(mapId);
	if (itr != loadedMMaps.cend() && !itr->second)
		itr = loadedMMaps.cend();

	return itr;
}

bool MMapMgrModule::loadMapData(uint32 mapId)
{
	// we already have this map loaded?
	MMapDataSet::iterator itr = loadedMMaps.find(mapId);
	if (itr != loadedMMaps.end())
	{
		if (itr->second)
			return true;
	}
	else
	{
		if (thread_safe_environment)
			itr = loadedMMaps.insert(MMapDataSet::value_type(mapId, nullptr)).first;
		else
			ASSERT(false, "Invalid mapId %u passed to MMapMgrModule after startup in thread unsafe environment", mapId);
	}

	// load and init dtNavMesh - read parameters from file
	char buf[128] = { 0 };
	snprintf(buf, sizeof(buf) - 1, MAP_FILE_NAME_FORMAT, ".", mapId);
	std::string fileName = buf;
	FILE* file = fopen(fileName.c_str(), "rb");
	if (!file)
	{
		LOG_WARN << "MMAP:loadMapData: Error: Could not open mmap file:" << fileName;
		return false;
	}

	dtNavMeshParams params;
	uint32 count = uint32(fread(&params, sizeof(dtNavMeshParams), 1, file));
	fclose(file);
	if (count != 1)
	{
		LOG_WARN << "MMAP:loadMapData: Error: Could not read params from file: " << fileName;
		return false;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	ASSERT(mesh);
	if (dtStatusFailed(mesh->init(&params)))
	{
		dtFreeNavMesh(mesh);
		LOG_ERROR << fmt::format("Failed to initialize dtNavMesh for mmap {} from file {}", mapId, fileName.c_str());
		return false;
	}

	// store inside our map list
	MMapData* mmap_data = new MMapData(mesh);

	itr->second = mmap_data;
	return true;
}

uint32 MMapMgrModule::packTileID(int32 x, int32 y)
{
	return uint32(x << 16 | y);
}

bool MMapMgrModule::loadMap(const std::string& /*basePath*/, uint32 mapId, int32 x, int32 y)
{
	// make sure the mmap is loaded and ready to load tiles
	if (!loadMapData(mapId))
		return false;

	// get this mmap data
	MMapData* mmap = loadedMMaps[mapId];
	ASSERT(mmap->navMesh);

	// check if we already have this tile loaded
	uint32 packedGridPos = packTileID(x, y);
	if (mmap->loadedTileRefs.find(packedGridPos) != mmap->loadedTileRefs.end())
		return false;

	// load this tile :: mmaps/MMMXXYY.mmtile
	char buf[128] = { 0 };
	snprintf(buf, sizeof(buf) - 1, TILE_FILE_NAME_FORMAT, ".", mapId, x, y);
	std::string fileName = buf;
	FILE* file = fopen(fileName.c_str(), "rb");
	if (!file)
	{
		LOG_WARN << "MMAP:loadMap: Could not open mmtile file: " << fileName.c_str();
		return false;
	}

	// read header
	MmapTileHeader fileHeader;
	if (fread(&fileHeader, sizeof(MmapTileHeader), 1, file) != 1 || fileHeader.mmapMagic != MMAP_MAGIC)
	{
		LOG_ERROR << fmt::format("MMAP:loadMap: Bad header in mmap {}.{}.{}.mmtile", mapId, x, y);
		fclose(file);
		return false;
	}

	if (fileHeader.mmapVersion != MMAP_VERSION)
	{
		LOG_ERROR << fmt::format("MMAP:loadMap: {}.{}.{}.mmtile was built with generator v{}, expected v{}",
			mapId, x, y, fileHeader.mmapVersion, MMAP_VERSION);
		fclose(file);
		return false;
	}

	long pos = ftell(file);
	fseek(file, 0, SEEK_END);
	if (pos < 0 || static_cast<int32>(fileHeader.size) > ftell(file) - pos)
	{
		LOG_ERROR << fmt::format("MMAP:loadMap: {}.{}.mmtile has corrupted data size", mapId, x, y);
		fclose(file);
		return false;
	}

	fseek(file, pos, SEEK_SET);

	unsigned char* data = (unsigned char*)dtAlloc(fileHeader.size, DT_ALLOC_PERM);
	ASSERT(data);

	size_t result = fread(data, fileHeader.size, 1, file);
	if (!result)
	{
		LOG_ERROR << fmt::format("MMAP:loadMap: Bad header or data in mmap {}.{}.{}.mmtile", mapId, x, y);
		fclose(file);
		return false;
	}

	fclose(file);

	dtMeshHeader* header = (dtMeshHeader*)data;
	dtTileRef tileRef = 0;

	// memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
	if (dtStatusSucceed(mmap->navMesh->addTile(data, fileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef)))
	{
		mmap->loadedTileRefs.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
		++loadedTiles;
		LOG_WARN << fmt::format("MMAP:loadMap: Loaded mmtile {}[{}, {}] into{}[{}, {}]", mapId, x, y, mapId, header->x, header->y);
		return true;
	}
	else
	{
		LOG_ERROR << fmt::format("MMAP:loadMap: Could not load {}.{}.{}.mmtile into navmesh", mapId, x, y);
		dtFree(data);
		return false;
	}
}

bool MMapMgrModule::unloadMap(uint32 mapId, int32 x, int32 y)
{
	// check if we have this map loaded
	MMapDataSet::const_iterator itr = GetMMapData(mapId);
	if (itr == loadedMMaps.end())
	{
		// file may not exist, therefore not loaded
		LOG_WARN << fmt::format("MMAP:unloadMap: Asked to unload not loaded navmesh map. {}.{}.{}.mmtile", mapId, x, y);
		return false;
	}

	MMapData* mmap = itr->second;

	// check if we have this tile loaded
	uint32 packedGridPos = packTileID(x, y);
	if (mmap->loadedTileRefs.find(packedGridPos) == mmap->loadedTileRefs.end())
	{
		// file may not exist, therefore not loaded
		LOG_WARN << fmt::format("MMAP:unloadMap: Asked to unload not loaded navmesh tile. {}.{}.{}.mmtile", mapId, x, y);
		return false;
	}

	dtTileRef tileRef = mmap->loadedTileRefs[packedGridPos];

	// unload, and mark as non loaded
	if (dtStatusFailed(mmap->navMesh->removeTile(tileRef, nullptr, nullptr)))
	{
		// this is technically a memory leak
		// if the grid is later reloaded, dtNavMesh::addTile will return error but no extra memory is used
		// we cannot recover from this error - assert out
		LOG_ERROR << fmt::format("MMAP:unloadMap: Could not unload {}.{}.{}.mmtile from navmesh", mapId, x, y);
		ASSERT(false);
		return false;
	}
	else
	{
		mmap->loadedTileRefs.erase(packedGridPos);
		--loadedTiles;
		LOG_WARN << fmt::format("MMAP:unloadMap: Unloaded mmtile {}[{}, {}] from {}", mapId, x, y, mapId);
		return true;
	}

	return false;
}

bool MMapMgrModule::unloadMap(uint32 mapId)
{
	MMapDataSet::iterator itr = loadedMMaps.find(mapId);
	if (itr == loadedMMaps.end() || !itr->second)
	{
		// file may not exist, therefore not loaded
		LOG_WARN << fmt::format("MMAP:unloadMap: Asked to unload not loaded navmesh map {}", mapId);
		return false;
	}

	// unload all tiles from given map
	MMapData* mmap = itr->second;
	for (MMapTileSet::iterator i = mmap->loadedTileRefs.begin(); i != mmap->loadedTileRefs.end(); ++i)
	{
		uint32 x = (i->first >> 16);
		uint32 y = (i->first & 0x0000FFFF);
		if (dtStatusFailed(mmap->navMesh->removeTile(i->second, nullptr, nullptr)))
			LOG_ERROR << fmt::format("MMAP:unloadMap: Could not unload {}.{}.{}.mmtile from navmesh", mapId, x, y);
		else
		{
			--loadedTiles;
			LOG_WARN << fmt::format("MMAP:unloadMap: Unloaded mmtile {}[{}, {}] from %03i", mapId, x, y, mapId);
		}
	}

	delete mmap;
	itr->second = nullptr;
	LOG_WARN << fmt::format("MMAP:unloadMap: Unloaded %03i.mmap", mapId);

	return true;
}

bool MMapMgrModule::unloadMapInstance(uint32 mapId, uint32 instanceId)
{
	// check if we have this map loaded
	MMapDataSet::const_iterator itr = GetMMapData(mapId);
	if (itr == loadedMMaps.end())
	{
		// file may not exist, therefore not loaded
		LOG_WARN << fmt::format("MMAP:unloadMapInstance: Asked to unload not loaded navmesh map {}", mapId);
		return false;
	}

	MMapData* mmap = itr->second;
	if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
	{
		LOG_WARN << fmt::format("Asked to unload not loaded dtNavMeshQuery mapId {} instanceId {}", mapId, instanceId);
		return false;
	}

	dtNavMeshQuery* query = mmap->navMeshQueries[instanceId];

	dtFreeNavMeshQuery(query);
	mmap->navMeshQueries.erase(instanceId);
	LOG_WARN << fmt::format("MMAP:unloadMapInstance: Unloaded mapId {} instanceId {}", mapId, instanceId);

	return true;
}

dtNavMesh const* MMapMgrModule::GetNavMesh(uint32 mapId)
{
	MMapDataSet::const_iterator itr = GetMMapData(mapId);
	if (itr == loadedMMaps.end())
		return nullptr;

	return itr->second->navMesh;
}

dtNavMeshQuery const* MMapMgrModule::GetNavMeshQuery(uint32 mapId, uint32 instanceId)
{
	MMapDataSet::const_iterator itr = GetMMapData(mapId);
	if (itr == loadedMMaps.end())
		return nullptr;

	MMapData* mmap = itr->second;
	if (mmap->navMeshQueries.find(instanceId) == mmap->navMeshQueries.end())
	{
		// allocate mesh query
		dtNavMeshQuery* query = dtAllocNavMeshQuery();
		ASSERT(query);
		if (dtStatusFailed(query->init(mmap->navMesh, 1024)))
		{
			dtFreeNavMeshQuery(query);
			LOG_ERROR << fmt::format("MMAP:GetNavMeshQuery: Failed to initialize dtNavMeshQuery for mapId {} instanceId {}", mapId, instanceId);
			return nullptr;
		}

		LOG_WARN << fmt::format("MMAP:GetNavMeshQuery: created dtNavMeshQuery for mapId {} instanceId {}", mapId, instanceId);
		mmap->navMeshQueries.insert(std::pair<uint32, dtNavMeshQuery*>(instanceId, query));
	}

	return mmap->navMeshQueries[instanceId];
}


}

