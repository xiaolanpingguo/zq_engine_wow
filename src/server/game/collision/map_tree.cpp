#include "map_tree.h"
#include "model_instance.h"
#include "map_defines.h"
#include "interface_header/logic/IVMapMgrModule.h"
//#include "Metric.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

using G3D::Vector3;

namespace zq{

class MapRayCallback
{
public:
	MapRayCallback(ModelInstance* val, ModelIgnoreFlags ignoreFlags) : prims(val), hit(false), flags(ignoreFlags) { }
	bool operator()(const G3D::Ray& ray, uint32 entry, float& distance, bool pStopAtFirstHit = true)
	{
		bool result = prims[entry].intersectRay(ray, distance, pStopAtFirstHit, flags);
		if (result)
			hit = true;
		return result;
	}
    bool didHit() { return hit; }
protected:
    ModelInstance* prims;
    bool hit;
    ModelIgnoreFlags flags;
};

class AreaInfoCallback
{
public:
    AreaInfoCallback(ModelInstance* val): prims(val) { }
    void operator()(Vector3 const& point, uint32 entry)
    {
        prims[entry].intersectPoint(point, aInfo);
    }

    ModelInstance* prims;
    AreaInfo aInfo;
};

class LocationInfoCallback
{
public:
	LocationInfoCallback(ModelInstance* val, LocationInfo &info) : prims(val), locInfo(info), result(false) { }
	void operator()(Vector3 const& point, uint32 entry)
	{
		if (prims[entry].GetLocationInfo(point, locInfo))
			result = true;
	}

	ModelInstance* prims;
	LocationInfo &locInfo;
	bool result;
};

//=========================================================

std::string StaticMapTree::getTileFileName(uint32 mapID, uint32 tileX, uint32 tileY)
{
    std::stringstream tilefilename;
    tilefilename.fill('0');
    tilefilename << std::setw(3) << mapID << '_';
    //tilefilename << std::setw(2) << tileX << '_' << std::setw(2) << tileY << ".vmtile";
    tilefilename << std::setw(2) << tileY << '_' << std::setw(2) << tileX << ".vmtile";
    return tilefilename.str();
}

bool StaticMapTree::getAreaInfo(Vector3 &pos, uint32 &flags, int32 &adtId, int32 &rootId, int32 &groupId) const
{
    AreaInfoCallback intersectionCallBack(iTreeValues);
    iTree.intersectPoint(pos, intersectionCallBack);
    if (intersectionCallBack.aInfo.result)
    {
        flags = intersectionCallBack.aInfo.flags;
        adtId = intersectionCallBack.aInfo.adtId;
        rootId = intersectionCallBack.aInfo.rootId;
        groupId = intersectionCallBack.aInfo.groupId;
        pos.z = intersectionCallBack.aInfo.ground_Z;
        return true;
    }
    return false;
}

bool StaticMapTree::GetLocationInfo(Vector3 const& pos, LocationInfo &info) const
{
    LocationInfoCallback intersectionCallBack(iTreeValues, info);
    iTree.intersectPoint(pos, intersectionCallBack);
    return intersectionCallBack.result;
}

StaticMapTree::StaticMapTree(uint32 mapID, std::string const& basePath) :
    iMapID(mapID), iIsTiled(false), iTreeValues(nullptr),
    iNTreeValues(0), iBasePath(basePath)
{
    if (iBasePath.length() > 0 && iBasePath[iBasePath.length()-1] != '/' && iBasePath[iBasePath.length()-1] != '\\')
    {
        iBasePath.push_back('/');
    }
}

//=========================================================
//! Make sure to call unloadMap() to unregister acquired model references before destroying
StaticMapTree::~StaticMapTree()
{
    delete[] iTreeValues;
}

//=========================================================
/**
If intersection is found within pMaxDist, sets pMaxDist to intersection distance and returns true.
Else, pMaxDist is not modified and returns false;
*/
bool StaticMapTree::getIntersectionTime(const G3D::Ray& pRay, float &pMaxDist, bool pStopAtFirstHit, ModelIgnoreFlags ignoreFlags) const
{
    float distance = pMaxDist;
    MapRayCallback intersectionCallBack(iTreeValues, ignoreFlags);
    iTree.intersectRay(pRay, intersectionCallBack, distance, pStopAtFirstHit);
    if (intersectionCallBack.didHit())
        pMaxDist = distance;
    return intersectionCallBack.didHit();
}

//=========================================================
bool StaticMapTree::isInLineOfSight(Vector3 const& pos1, Vector3 const& pos2, ModelIgnoreFlags ignoreFlag) const
{
    float maxDist = (pos2 - pos1).magnitude();
    // return false if distance is over max float, in case of cheater teleporting to the end of the universe
    if (maxDist == std::numeric_limits<float>::max() || !std::isfinite(maxDist))
        return false;

    // valid map coords should *never ever* produce float overflow, but this would produce NaNs too
    ASSERT(maxDist < std::numeric_limits<float>::max());
    // prevent NaN values which can cause BIH intersection to enter infinite loop
    if (maxDist < 1e-10f)
        return true;
    // direction with length of 1
    G3D::Ray ray = G3D::Ray::fromOriginAndDirection(pos1, (pos2 - pos1)/maxDist);
    if (getIntersectionTime(ray, maxDist, true, ignoreFlag))
        return false;

    return true;
}
//=========================================================
/**
When moving from pos1 to pos2 check if we hit an object. Return true and the position if we hit one
Return the hit pos or the original dest pos
*/

bool StaticMapTree::getObjectHitPos(Vector3 const& pPos1, Vector3 const& pPos2, Vector3& pResultHitPos, float pModifyDist) const
{
    bool result = false;
    float maxDist = (pPos2 - pPos1).magnitude();
    // valid map coords should *never ever* produce float overflow, but this would produce NaNs too
    ASSERT(maxDist < std::numeric_limits<float>::max());
    // prevent NaN values which can cause BIH intersection to enter infinite loop
    if (maxDist < 1e-10f)
    {
        pResultHitPos = pPos2;
        return false;
    }
    Vector3 dir = (pPos2 - pPos1)/maxDist;              // direction with length of 1
    G3D::Ray ray(pPos1, dir);
    float dist = maxDist;
    if (getIntersectionTime(ray, dist, false, ModelIgnoreFlags::Nothing))
    {
        pResultHitPos = pPos1 + dir * dist;
        if (pModifyDist < 0)
        {
            if ((pResultHitPos - pPos1).magnitude() > -pModifyDist)
            {
                pResultHitPos = pResultHitPos + dir*pModifyDist;
            }
            else
            {
                pResultHitPos = pPos1;
            }
        }
        else
        {
            pResultHitPos = pResultHitPos + dir*pModifyDist;
        }
        result = true;
    }
    else
    {
        pResultHitPos = pPos2;
        result = false;
    }
    return result;
}

//=========================================================

float StaticMapTree::getHeight(Vector3 const& pPos, float maxSearchDist) const
{
    float height = G3D::finf();
    Vector3 dir = Vector3(0, 0, -1);
    G3D::Ray ray(pPos, dir);   // direction with length of 1
    float maxDist = maxSearchDist;
    if (getIntersectionTime(ray, maxDist, false, ModelIgnoreFlags::Nothing))
    {
        height = pPos.z - maxDist;
    }
    return(height);
}

//=========================================================
LoadResult StaticMapTree::CanLoadMap(const std::string &vmapPath, uint32 mapID, uint32 tileX, uint32 tileY)
{
	LoadResult result = LoadResult::Success;

	/* std::string basePath = vmapPath;
	 if (basePath.length() > 0 && basePath[basePath.length()-1] != '/' && basePath[basePath.length()-1] != '\\')
		 basePath.push_back('/');
	 std::string fullname = basePath + VMapManager2::getMapFileName(mapID);

	 FILE* rf = fopen(fullname.c_str(), "rb");
	 if (!rf)
		 return LoadResult::FileNotFound;

	 char tiled;
	 char chunk[8];
	 if (!readChunk(rf, chunk, VMAP_MAGIC, 8) || fread(&tiled, sizeof(char), 1, rf) != 1)
	 {
		 fclose(rf);
		 return LoadResult::VersionMismatch;
	 }
	 if (tiled)
	 {
		 std::string tilefile = basePath + getTileFileName(mapID, tileX, tileY);
		 FILE* tf = fopen(tilefile.c_str(), "rb");
		 if (!tf)
			 result = LoadResult::FileNotFound;
		 else
		 {
			 if (!readChunk(tf, chunk, VMAP_MAGIC, 8))
				 result = LoadResult::VersionMismatch;
			 fclose(tf);
		 }
	 }
	 fclose(rf);*/
    return result;
}

//=========================================================

bool StaticMapTree::InitMap(const std::string &fname)
{
    bool success = false;
    std::string fullname = iBasePath + fname;
    FILE* rf = fopen(fullname.c_str(), "rb");
    if (!rf)
        return false;

    char chunk[8];
    char tiled = '\0';

    if (readChunk(rf, chunk, VMAP_MAGIC, 8) && fread(&tiled, sizeof(char), 1, rf) == 1 &&
        readChunk(rf, chunk, "NODE", 4) && iTree.readFromFile(rf))
    {
        iNTreeValues = iTree.primCount();
        iTreeValues = new ModelInstance[iNTreeValues];
        success = readChunk(rf, chunk, "GOBJ", 4);
    }

    iIsTiled = tiled != '\0';

    // global model spawns
    // only non-tiled maps have them, and if so exactly one (so far at least...)
    ModelSpawn spawn;
    if (!iIsTiled && ModelSpawn::readFromFile(rf, spawn))
    {
        //WorldModel* model = vm->acquireModelInstance(iBasePath, spawn.name, spawn.flags);
        //if (model)
        //{
        //    // assume that global model always is the first and only tree value (could be improved...)
        //    iTreeValues[0] = ModelInstance(spawn, model);
        //    iLoadedSpawns[0] = 1;
        //}
        //else
        //{
        //    success = false;
        //    LOG_ERROR << "StaticMapTree::InitMap() : could not acquire WorldModel pointer for " << spawn.name.c_str();
        //}
    }

    fclose(rf);
    return success;
}

//=========================================================

void StaticMapTree::UnloadMap()
{
    for (loadedSpawnMap::iterator i = iLoadedSpawns.begin(); i != iLoadedSpawns.end(); ++i)
    {
        iTreeValues[i->first].setUnloaded();
		/*for (uint32 refCount = 0; refCount < i->second; ++refCount)
			vm->releaseModelInstance(iTreeValues[i->first].name);*/
    }
    iLoadedSpawns.clear();
    iLoadedTiles.clear();
}

//=========================================================

bool StaticMapTree::LoadMapTile(uint32 tileX, uint32 tileY)
{
    if (!iIsTiled)
    {
        // currently, core creates grids for all maps, whether it has terrain tiles or not
        // so we need "fake" tile loads to know when we can unload map geometry
        iLoadedTiles[packTileID(tileX, tileY)] = false;
        return true;
    }
    if (!iTreeValues)
    {
        LOG_ERROR << fmt::format("StaticMapTree::LoadMapTile() : tree has not been initialized [{}, {}]", tileX, tileY);
        return false;
    }
    bool result = true;

    std::string tilefile = iBasePath + getTileFileName(iMapID, tileX, tileY);
    FILE* tf = fopen(tilefile.c_str(), "rb");
    if (tf)
    {
        char chunk[8];

        if (!readChunk(tf, chunk, VMAP_MAGIC, 8))
            result = false;
        uint32 numSpawns = 0;
        if (result && fread(&numSpawns, sizeof(uint32), 1, tf) != 1)
            result = false;
        for (uint32 i=0; i<numSpawns && result; ++i)
        {
            // read model spawns
            ModelSpawn spawn;
            result = ModelSpawn::readFromFile(tf, spawn);
            if (result)
            {
                // acquire model instance
			   /* WorldModel* model = vm->acquireModelInstance(iBasePath, spawn.name, spawn.flags);
				if (!model)
					LOG_ERROR << fmt::format("StaticMapTree::LoadMapTile() : could not acquire WorldModel pointer [{}, {}]", tileX, tileY);*/

                // update tree
                uint32 referencedVal;

                if (fread(&referencedVal, sizeof(uint32), 1, tf) == 1)
                {
                    if (!iLoadedSpawns.count(referencedVal))
                    {
                        if (referencedVal > iNTreeValues)
                        {
							LOG_ERROR << fmt::format("StaticMapTree::LoadMapTile() : invalid tree element ({}/{}) referenced in tile {}", referencedVal, iNTreeValues, tilefile.c_str());
                            continue;
                        }

						/*iTreeValues[referencedVal] = ModelInstance(spawn, model);
						iLoadedSpawns[referencedVal] = 1;*/
                    }
                    else
                    {
                        ++iLoadedSpawns[referencedVal];
                    }
                }
                else
                    result = false;
            }
        }
        iLoadedTiles[packTileID(tileX, tileY)] = true;
        fclose(tf);
    }
    else
        iLoadedTiles[packTileID(tileX, tileY)] = false;

    return result;
}

//=========================================================

void StaticMapTree::UnloadMapTile(uint32 tileX, uint32 tileY)
{
    uint32 tileID = packTileID(tileX, tileY);
    loadedTileMap::iterator tile = iLoadedTiles.find(tileID);
    if (tile == iLoadedTiles.end())
    {
		LOG_ERROR << fmt::format("StaticMapTree::UnloadMapTile() : trying to unload non-loaded tile - Map:{} X:{} Y:{}", iMapID, tileX, tileY);
        return;
    }
    if (tile->second) // file associated with tile
    {
        std::string tilefile = iBasePath + getTileFileName(iMapID, tileX, tileY);
        FILE* tf = fopen(tilefile.c_str(), "rb");
        if (tf)
        {
            bool result = true;
            char chunk[8];
            if (!readChunk(tf, chunk, VMAP_MAGIC, 8))
                result = false;
            uint32 numSpawns;
            if (fread(&numSpawns, sizeof(uint32), 1, tf) != 1)
                result = false;
            for (uint32 i=0; i<numSpawns && result; ++i)
            {
                // read model spawns
                ModelSpawn spawn;
                result = ModelSpawn::readFromFile(tf, spawn);
                if (result)
                {
                    // release model instance
                   // vm->releaseModelInstance(spawn.name);

                    // update tree
                    uint32 referencedNode;

                    if (fread(&referencedNode, sizeof(uint32), 1, tf) != 1)
                        result = false;
                    else
                    {
                        if (!iLoadedSpawns.count(referencedNode))
							LOG_ERROR << fmt::format("StaticMapTree::UnloadMapTile() : trying to unload non-referenced model '{}' (ID:{})", spawn.name.c_str(), spawn.ID);
                        else if (--iLoadedSpawns[referencedNode] == 0)
                        {
                            iTreeValues[referencedNode].setUnloaded();
                            iLoadedSpawns.erase(referencedNode);
                        }
                    }
                }
            }
            fclose(tf);
        }
    }

    iLoadedTiles.erase(tile);
}

void StaticMapTree::getModelInstances(ModelInstance* &models, uint32 &count)
{
    models = iTreeValues;
    count = iNTreeValues;
}
}
