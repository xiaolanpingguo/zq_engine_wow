#include "vmap_mgr_module.h"
#include "../collision/map_tree.h"
#include "world_model.h"

#include <mutex>
#include <G3D/Vector3.h>
using namespace G3D;

namespace zq {


#define MAP_FILENAME_EXTENSION2 ".vmtree"
#define FILENAMEBUFFER_SIZE 500


bool VMapMgrModule::init()
{
	//objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	//configModule_ = libManager_->findModule<IConfigModule>();

	GetLiquidFlagsPtr = &GetLiquidFlagsDummy;
	IsVMAPDisabledForPtr = &IsVMAPDisabledForDummy;
	thread_safe_environment = true;

	return true;
}

bool VMapMgrModule::initEnd()
{

	return true;
}

bool VMapMgrModule::run()
{

	return true;
}

bool VMapMgrModule::finalize()
{
	for (InstanceTreeMap::iterator i = iInstanceMapTrees.begin(); i != iInstanceMapTrees.end(); ++i)
	{
		delete i->second;
	}
	for (ModelFileMap::iterator i = iLoadedModelFiles.begin(); i != iLoadedModelFiles.end(); ++i)
	{
		delete i->second.getModel();
	}

	return true;
}

void VMapMgrModule::InitializeThreadUnsafe(const std::vector<uint32>& mapIds)
{
	// the caller must pass the list of all mapIds that will be used in the VMapMgrModule lifetime
	for (uint32 const& mapId : mapIds)
		iInstanceMapTrees.insert(InstanceTreeMap::value_type(mapId, nullptr));

	thread_safe_environment = false;
}

G3D::Vector3 VMapMgrModule::convertPositionToInternalRep(float x, float y, float z) const
{
	G3D::Vector3 pos;
	const float mid = 0.5f * 64.0f * 533.33333333f;
	pos.x = mid - x;
	pos.y = mid - y;
	pos.z = z;

	return pos;
}

// move to MapTree too?
std::string VMapMgrModule::getMapFileName(unsigned int mapId)
{
	std::stringstream fname;
	fname.width(3);
	fname << std::setfill('0') << mapId << std::string(MAP_FILENAME_EXTENSION2);

	return fname.str();
}

int VMapMgrModule::loadMap(char const* basePath, unsigned int mapId, int x, int y)
{
	int result = VMAP_LOAD_RESULT_IGNORED;
	if (isMapLoadingEnabled())
	{
		if (_loadMap(mapId, basePath, x, y))
			result = VMAP_LOAD_RESULT_OK;
		else
			result = VMAP_LOAD_RESULT_ERROR;
	}

	return result;
}

InstanceTreeMap::const_iterator VMapMgrModule::GetMapTree(uint32 mapId) const
{
	// return the iterator if found or end() if not found/NULL
	InstanceTreeMap::const_iterator itr = iInstanceMapTrees.find(mapId);
	if (itr != iInstanceMapTrees.cend() && !itr->second)
		itr = iInstanceMapTrees.cend();

	return itr;
}

// load one tile (internal use only)
bool VMapMgrModule::_loadMap(uint32 mapId, const std::string& basePath, uint32 tileX, uint32 tileY)
{
	InstanceTreeMap::iterator instanceTree = iInstanceMapTrees.find(mapId);
	if (instanceTree == iInstanceMapTrees.end())
	{
		if (thread_safe_environment)
			instanceTree = iInstanceMapTrees.insert(InstanceTreeMap::value_type(mapId, nullptr)).first;
		else
			ASSERT(false, "Invalid mapId %u tile [%u, %u] passed to VMapMgrModule after startup in thread unsafe environment",
				mapId, tileX, tileY);
	}

	if (!instanceTree->second)
	{
		std::string mapFileName = getMapFileName(mapId);
		StaticMapTree* newTree = new StaticMapTree(mapId, basePath);
		/*if (!newTree->InitMap(mapFileName, this))
		{
			delete newTree;
			return false;
		}*/
		instanceTree->second = newTree;
	}

	//return instanceTree->second->LoadMapTile(tileX, tileY, this);
	return instanceTree->second->LoadMapTile(tileX, tileY);
}

void VMapMgrModule::unloadMap(unsigned int mapId)
{
	InstanceTreeMap::iterator instanceTree = iInstanceMapTrees.find(mapId);
	if (instanceTree != iInstanceMapTrees.end() && instanceTree->second)
	{
		/*instanceTree->second->UnloadMap(this);
		if (instanceTree->second->numLoadedTiles() == 0)
		{
			delete instanceTree->second;
			instanceTree->second = nullptr;
		}*/
	}
}

void VMapMgrModule::unloadMap(unsigned int mapId, int x, int y)
{
	InstanceTreeMap::iterator instanceTree = iInstanceMapTrees.find(mapId);
	if (instanceTree != iInstanceMapTrees.end() && instanceTree->second)
	{
		/*instanceTree->second->UnloadMapTile(x, y, this);
		if (instanceTree->second->numLoadedTiles() == 0)
		{
			delete instanceTree->second;
			instanceTree->second = nullptr;
		}*/
	}
}

bool VMapMgrModule::isInLineOfSight(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2, ModelIgnoreFlags ignoreFlags)
{
	if (!isLineOfSightCalcEnabled() || IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LOS))
		return true;

	InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
	if (instanceTree != iInstanceMapTrees.end())
	{
		Vector3 pos1 = convertPositionToInternalRep(x1, y1, z1);
		Vector3 pos2 = convertPositionToInternalRep(x2, y2, z2);
		if (pos1 != pos2)
		{
			return instanceTree->second->isInLineOfSight(pos1, pos2, ignoreFlags);
		}
	}

	return true;
}

/**
get the hit position and return true if we hit something
otherwise the result pos will be the dest pos
*/
bool VMapMgrModule::getObjectHitPos(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float &ry, float& rz, float modifyDist)
{
	if (isLineOfSightCalcEnabled() && !IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LOS))
	{
		InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
		if (instanceTree != iInstanceMapTrees.end())
		{
			Vector3 pos1 = convertPositionToInternalRep(x1, y1, z1);
			Vector3 pos2 = convertPositionToInternalRep(x2, y2, z2);
			Vector3 resultPos;
			bool result = instanceTree->second->getObjectHitPos(pos1, pos2, resultPos, modifyDist);
			resultPos = convertPositionToInternalRep(resultPos.x, resultPos.y, resultPos.z);
			rx = resultPos.x;
			ry = resultPos.y;
			rz = resultPos.z;
			return result;
		}
	}

	rx = x2;
	ry = y2;
	rz = z2;

	return false;
}

/**
get height or INVALID_HEIGHT if no height available
*/

float VMapMgrModule::getHeight(unsigned int mapId, float x, float y, float z, float maxSearchDist)
{
	if (isHeightCalcEnabled() && !IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_HEIGHT))
	{
		InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
		if (instanceTree != iInstanceMapTrees.end())
		{
			Vector3 pos = convertPositionToInternalRep(x, y, z);
			float height = instanceTree->second->getHeight(pos, maxSearchDist);
			if (!(height < G3D::finf()))
				return height = VMAP_INVALID_HEIGHT_VALUE; // No height

			return height;
		}
	}

	return VMAP_INVALID_HEIGHT_VALUE;
}

bool VMapMgrModule::getAreaInfo(uint32 mapId, float x, float y, float& z, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const
{
	if (!IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_AREAFLAG))
	{
		InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
		if (instanceTree != iInstanceMapTrees.end())
		{
			Vector3 pos = convertPositionToInternalRep(x, y, z);
			bool result = instanceTree->second->getAreaInfo(pos, flags, adtId, rootId, groupId);
			// z is not touched by convertPositionToInternalRep(), so just copy
			z = pos.z;
			return result;
		}
	}

	return false;
}

bool VMapMgrModule::GetLiquidLevel(uint32 mapId, float x, float y, float z, uint8 reqLiquidType, float& level, float& floor, uint32& type, uint32& mogpFlags) const
{
	if (!IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LIQUIDSTATUS))
	{
		InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
		if (instanceTree != iInstanceMapTrees.end())
		{
			LocationInfo info;
			Vector3 pos = convertPositionToInternalRep(x, y, z);
			if (instanceTree->second->GetLocationInfo(pos, info))
			{
				floor = info.ground_Z;
				ASSERT(floor < std::numeric_limits<float>::max());
				ASSERT(info.hitModel);
				type = info.hitModel->GetLiquidType();  // entry from LiquidType.dbc
				mogpFlags = info.hitModel->GetMogpFlags();
				if (reqLiquidType && !(GetLiquidFlagsPtr(type) & reqLiquidType))
					return false;
				ASSERT(info.hitInstance);
				if (info.hitInstance->GetLiquidLevel(pos, info, level))
					return true;
			}
		}
	}

	return false;
}

void VMapMgrModule::getAreaAndLiquidData(unsigned int mapId, float x, float y, float z, uint8 reqLiquidType, AreaAndLiquidData& data) const
{
	if (IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_LIQUIDSTATUS))
	{
		data.floorZ = z;
		int32 adtId, rootId, groupId;
		uint32 flags;
		//if (getAreaInfo(mapId, x, y, data.floorZ, flags, adtId, rootId, groupId))
		//	data.areaInfo = boost::in_place(adtId, rootId, groupId, flags);

		return;
	}
	InstanceTreeMap::const_iterator instanceTree = GetMapTree(mapId);
	if (instanceTree != iInstanceMapTrees.end())
	{
		LocationInfo info;
		Vector3 pos = convertPositionToInternalRep(x, y, z);
		if (instanceTree->second->GetLocationInfo(pos, info))
		{
			ASSERT(info.hitModel);
			ASSERT(info.hitInstance);
			data.floorZ = info.ground_Z;
			uint32 liquidType = info.hitModel->GetLiquidType();
			float liquidLevel;
			//if (!reqLiquidType || (GetLiquidFlagsPtr(liquidType) & reqLiquidType))
			//	if (info.hitInstance->GetLiquidLevel(pos, info, liquidLevel))
			//		data.liquidInfo = boost::in_place(liquidType, liquidLevel);

			//if (!IsVMAPDisabledForPtr(mapId, VMAP_DISABLE_AREAFLAG))
			//	data.areaInfo = boost::in_place(info.hitInstance->adtId, info.rootId, info.hitModel->GetWmoID(), info.hitModel->GetMogpFlags());
		}
	}
}

WorldModel* VMapMgrModule::acquireModelInstance(const std::string& basepath, const std::string& filename, uint32 flags/* Only used when creating the model */)
{
	//! Critical section, thread safe access to iLoadedModelFiles
	std::lock_guard<std::mutex> lock(LoadedModelFilesLock);

	ModelFileMap::iterator model = iLoadedModelFiles.find(filename);
	if (model == iLoadedModelFiles.end())
	{
		WorldModel* worldmodel = new WorldModel();
		if (!worldmodel->readFile(basepath + filename + ".vmo"))
		{
			LOG_ERROR << fmt::format("VMapMgrModule: could not load '{}.{}.vmo'", basepath.c_str(), filename.c_str());
			delete worldmodel;
			return nullptr;
		}

		LOG_WARN << fmt::format("VMapMgrModule: loading file '{}.{}'", basepath.c_str(), filename.c_str());
		worldmodel->Flags = flags;

		model = iLoadedModelFiles.insert(std::pair<std::string, ManagedModel>(filename, ManagedModel())).first;
		model->second.setModel(worldmodel);
	}
	model->second.incRefCount();
	return model->second.getModel();
}

void VMapMgrModule::releaseModelInstance(const std::string &filename)
{
	//! Critical section, thread safe access to iLoadedModelFiles
	std::lock_guard<std::mutex> lock(LoadedModelFilesLock);

	ModelFileMap::iterator model = iLoadedModelFiles.find(filename);
	if (model == iLoadedModelFiles.end())
	{
		LOG_ERROR << "VMapMgrModule: trying to unload non-loaded file: " << filename.c_str();
		return;
	}
	if (model->second.decRefCount() == 0)
	{
		LOG_WARN << "VMapMgrModule: unloading file: " << filename.c_str();
		delete model->second.getModel();
		iLoadedModelFiles.erase(model);
	}
}

LoadResult VMapMgrModule::existsMap(char const* basePath, unsigned int mapId, int x, int y)
{
	return StaticMapTree::CanLoadMap(std::string(basePath), mapId, x, y);
}

void VMapMgrModule::getInstanceMapTree(InstanceTreeMap &instanceMapTree)
{
	instanceMapTree = iInstanceMapTrees;
}



}

