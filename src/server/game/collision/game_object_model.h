#pragma once


#include "interface_header/base/platform.h"
#include <G3D/Matrix3.h>
#include <G3D/Vector3.h>
#include <G3D/AABox.h>
#include <G3D/Ray.h>
#include <memory>

namespace zq{



class WorldModel;
enum class ModelIgnoreFlags : uint32;
class Entity;
struct GameObjectDisplayInfoEntry;

class GameObjectModelOwnerBase
{
public:
    virtual bool IsSpawned() const { return false; }
    virtual uint32 GetDisplayId() const { return 0; }
    virtual uint32 GetPhaseMask() const { return 0; }
    virtual G3D::Vector3 GetPosition() const { return G3D::Vector3::zero(); }
    virtual float GetOrientation() const { return 0.0f; }
    virtual float GetScale() const { return 1.0f; }
    virtual void DebugVisualizeCorner(G3D::Vector3 const& /*corner*/) const { }
    virtual ~GameObjectModelOwnerBase() { }
};

class GameObjectModel /*, public Intersectable*/
{
    GameObjectModel() : phasemask(0), iInvScale(0), iScale(0), iModel(nullptr) { }
public:
    std::string name;

    const G3D::AABox& getBounds() const { return iBound; }

    ~GameObjectModel();

    const G3D::Vector3& getPosition() const { return iPos;}

    /**    Enables\disables collision. */
    void disable() { phasemask = 0;}
    void enable(uint32 ph_mask) { phasemask = ph_mask;}

    bool isEnabled() const {return phasemask != 0;}

    bool intersectRay(const G3D::Ray& Ray, float& MaxDist, bool StopAtFirstHit, uint32 ph_mask, ModelIgnoreFlags ignoreFlags) const;

    static GameObjectModel* Create(std::unique_ptr<GameObjectModelOwnerBase> modelOwner, std::string const& dataPath);

    bool UpdatePosition();

private:
    bool initialize(std::unique_ptr<GameObjectModelOwnerBase> modelOwner, std::string const& dataPath);

    uint32 phasemask;
    G3D::AABox iBound;
    G3D::Matrix3 iInvRot;
    G3D::Vector3 iPos;
    float iInvScale;
    float iScale;
    WorldModel* iModel;
    std::unique_ptr<GameObjectModelOwnerBase> owner;
};

void LoadGameObjectModelList(std::string const& dataPath);

}
