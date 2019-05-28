#pragma once


#include "interface_header/logic/IVMapMgrModule.h"
#include <mutex>


namespace G3D
{
	class Vector3;
}

namespace zq {


class StaticMapTree;
class WorldModel;

class ManagedModel
{
public:
	ManagedModel() : iModel(nullptr), iRefCount(0) { }
	void setModel(WorldModel* model) { iModel = model; }
	WorldModel* getModel() { return iModel; }
	void incRefCount() { ++iRefCount; }
	int decRefCount() { return --iRefCount; }
protected:
	WorldModel* iModel;
	int iRefCount;
};

typedef std::unordered_map<uint32, StaticMapTree*> InstanceTreeMap;
typedef std::unordered_map<std::string, ManagedModel> ModelFileMap;

enum DisableTypes
{
	VMAP_DISABLE_AREAFLAG = 0x1,
	VMAP_DISABLE_HEIGHT = 0x2,
	VMAP_DISABLE_LOS = 0x4,
	VMAP_DISABLE_LIQUIDSTATUS = 0x8
};

class VMapMgrModule : public IVMapMgrModule
{
public:
	VMapMgrModule(ILibManager* p) { libManager_ = p; }

	bool init() override;
	bool initEnd() override;
	bool run() override;
	bool finalize() override;

	G3D::Vector3 convertPositionToInternalRep(float x, float y, float z) const;
	static std::string getMapFileName(unsigned int mapId);

	void InitializeThreadUnsafe(const std::vector<uint32>& mapIds);
	int loadMap(char const* pBasePath, unsigned int mapId, int x, int y) override;

	void unloadMap(unsigned int mapId, int x, int y) override;
	void unloadMap(unsigned int mapId) override;

	bool isInLineOfSight(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2, ModelIgnoreFlags ignoreFlags) override;
	/**
	fill the hit pos and return true, if an object was hit
	*/
	bool getObjectHitPos(unsigned int mapId, float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float& ry, float& rz, float modifyDist) override;
	float getHeight(unsigned int mapId, float x, float y, float z, float maxSearchDist) override;

	bool processCommand(char* /*command*/) override { return false; } // for debug and extensions

	bool getAreaInfo(uint32 mapId, float x, float y, float& z, uint32& flags, int32& adtId, int32& rootId, int32& groupId) const override;
	bool GetLiquidLevel(uint32 mapId, float x, float y, float z, uint8 reqLiquidType, float& level, float& floor, uint32& type, uint32& mogpFlags) const override;
	void getAreaAndLiquidData(uint32 mapId, float x, float y, float z, uint8 reqLiquidType, AreaAndLiquidData& data) const override;

	WorldModel* acquireModelInstance(const std::string& basepath, const std::string& filename, uint32 flags = 0);
	void releaseModelInstance(const std::string& filename);

	// what's the use of this? o.O
	virtual std::string getDirFileName(unsigned int mapId, int /*x*/, int /*y*/) const override
	{
		return getMapFileName(mapId);
	}
	virtual LoadResult existsMap(char const* basePath, unsigned int mapId, int x, int y) override;

	void getInstanceMapTree(InstanceTreeMap &instanceMapTree);

	typedef uint32(*GetLiquidFlagsFn)(uint32 liquidType);
	GetLiquidFlagsFn GetLiquidFlagsPtr;

	typedef bool(*IsVMAPDisabledForFn)(uint32 entry, uint8 flags);
	IsVMAPDisabledForFn IsVMAPDisabledForPtr;
protected:

	// Tree to check collision
	ModelFileMap iLoadedModelFiles;
	InstanceTreeMap iInstanceMapTrees;
	bool thread_safe_environment;
	// Mutex for iLoadedModelFiles
	std::mutex LoadedModelFilesLock;

	bool _loadMap(uint32 mapId, const std::string& basePath, uint32 tileX, uint32 tileY);
	/* void _unloadMap(uint32 pMapId, uint32 x, uint32 y); */

	static uint32 GetLiquidFlagsDummy(uint32) { return 0; }
	static bool IsVMAPDisabledForDummy(uint32 /*entry*/, uint8 /*flags*/) { return false; }

	InstanceTreeMap::const_iterator GetMapTree(uint32 mapId) const;

private:

};

}
