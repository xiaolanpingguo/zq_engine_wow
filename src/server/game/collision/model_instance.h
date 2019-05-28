#pragma once


#include "interface_header/base/platform.h"
#include <G3D/Matrix3.h>
#include <G3D/Vector3.h>
#include <G3D/AABox.h>
#include <G3D/Ray.h>


namespace zq{


enum ModelFlags
{
    MOD_M2 = 1,
    MOD_WORLDSPAWN = 1<<1,
    MOD_HAS_BOUND = 1<<2
};

enum class ModelIgnoreFlags : uint32
{
	Nothing = 0x00,
	M2 = 0x01
};

inline ModelIgnoreFlags operator&(ModelIgnoreFlags left, ModelIgnoreFlags right) { return ModelIgnoreFlags(uint32(left) & uint32(right)); }

class WorldModel;
struct AreaInfo;
struct LocationInfo;

class ModelSpawn
{
public:
    //mapID, tileX, tileY, Flags, ID, Pos, Rot, Scale, Bound_lo, Bound_hi, name
    uint32 flags;
    uint16 adtId;
    uint32 ID;
    G3D::Vector3 iPos;
    G3D::Vector3 iRot;
    float iScale;
    G3D::AABox iBound;
    std::string name;
    bool operator==(ModelSpawn const& other) const { return ID == other.ID; }
    //uint32 hashCode() const { return ID; }
    // temp?
    const G3D::AABox& getBounds() const { return iBound; }

    static bool readFromFile(FILE* rf, ModelSpawn &spawn);
    static bool writeToFile(FILE* rw, ModelSpawn const& spawn);
};

class ModelInstance: public ModelSpawn
{
public:
    ModelInstance(): iInvScale(0.0f), iModel(nullptr) { }
    ModelInstance(ModelSpawn const& spawn, WorldModel* model);
    void setUnloaded() { iModel = nullptr; }
    bool intersectRay(G3D::Ray const& pRay, float& pMaxDist, bool pStopAtFirstHit, ModelIgnoreFlags ignoreFlags) const;
    void intersectPoint(G3D::Vector3 const& p, AreaInfo &info) const;
    bool GetLocationInfo(G3D::Vector3 const& p, LocationInfo &info) const;
    bool GetLiquidLevel(G3D::Vector3 const& p, LocationInfo &info, float &liqHeight) const;
    WorldModel* getWorldModel() { return iModel; }
protected:
    G3D::Matrix3 iInvRot;
    float iInvScale;
    WorldModel* iModel;
};

} 

