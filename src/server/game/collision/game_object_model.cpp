#include "game_object_model.h"
#include "../common/timer.hpp"
#include "world_model.h"
#include "map_defines.h"

using G3D::Vector3;
using G3D::Ray;
using G3D::AABox;

namespace zq{

struct GameobjectModelData
{
    GameobjectModelData(std::string const& name_, AABox const& box) :
        bound(box), name(name_) { }

    AABox bound;
    std::string name;
};

typedef std::unordered_map<uint32, GameobjectModelData> ModelList;
ModelList model_list;

void LoadGameObjectModelList(std::string const& dataPath)
{
    uint32 oldMSTime = getMSTime();

    FILE* model_list_file = fopen((dataPath + "vmaps/" + GAMEOBJECT_MODELS).c_str(), "rb");
    if (!model_list_file)
    {
        LOG_ERROR << "Unable to open file, file:."<< GAMEOBJECT_MODELS;
        return;
    }

    uint32 name_length, displayId;
    char buff[500];
    while (true)
    {
        Vector3 v1, v2;
        if (fread(&displayId, sizeof(uint32), 1, model_list_file) != 1)
            if (feof(model_list_file))  // EOF flag is only set after failed reading attempt
                break;

        if (fread(&name_length, sizeof(uint32), 1, model_list_file) != 1
            || name_length >= sizeof(buff)
            || fread(&buff, sizeof(char), name_length, model_list_file) != name_length
            || fread(&v1, sizeof(Vector3), 1, model_list_file) != 1
            || fread(&v2, sizeof(Vector3), 1, model_list_file) != 1)
        {
			LOG_ERROR << "File '%s' seems to be corrupted! file:" << GAMEOBJECT_MODELS;
            break;
        }

        if (v1.isNaN() || v2.isNaN())
        {
            LOG_ERROR << fmt::format("File '{}' Model '{}' has invalid v1{} v2{} values!", 
				GAMEOBJECT_MODELS, std::string(buff, name_length).c_str(), v1.toString().c_str(), v2.toString().c_str());
            continue;
        }

        model_list.insert
        (
            ModelList::value_type(displayId, GameobjectModelData(std::string(buff, name_length), AABox(v1, v2)))
        );
    }

    fclose(model_list_file);
    LOG_INFO << fmt::format(">> Loaded {} Entity models in {} ms", uint32(model_list.size()), GetMSTimeDiffToNow(oldMSTime));
}

GameObjectModel::~GameObjectModel()
{
    //if (iModel)
    //    ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->releaseModelInstance(name);
}

bool GameObjectModel::initialize(std::unique_ptr<GameObjectModelOwnerBase> modelOwner, std::string const& dataPath)
{
    ModelList::const_iterator it = model_list.find(modelOwner->GetDisplayId());
    if (it == model_list.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        LOG_ERROR << fmt::format("Entity model {} has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    //iModel = ((VMAP::VMapManager2*)VMAP::VMapFactory::createOrGetVMapManager())->acquireModelInstance(dataPath + "vmaps/", it->second.name);

    //if (!iModel)
    //    return false;

    name = it->second.name;
    iPos = modelOwner->GetPosition();
    phasemask = modelOwner->GetPhaseMask();
    iScale = modelOwner->GetScale();
    iInvScale = 1.f / iScale;

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(modelOwner->GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    iBound = rotated_bounds + iPos;
    owner = std::move(modelOwner);

    return true;
}

GameObjectModel* GameObjectModel::Create(std::unique_ptr<GameObjectModelOwnerBase> modelOwner, std::string const& dataPath)
{
    GameObjectModel* mdl = new GameObjectModel();
    if (!mdl->initialize(std::move(modelOwner), dataPath))
    {
        delete mdl;
        return nullptr;
    }

    return mdl;
}

bool GameObjectModel::intersectRay(const G3D::Ray& ray, float& MaxDist, bool StopAtFirstHit, uint32 ph_mask, ModelIgnoreFlags ignoreFlags) const
{
    if (!(phasemask & ph_mask) || !owner->IsSpawned())
        return false;

    float time = ray.intersectionTime(iBound);
    if (time == G3D::finf())
        return false;

    // child bounds are defined in object space:
    Vector3 p = iInvRot * (ray.origin() - iPos) * iInvScale;
    Ray modRay(p, iInvRot * ray.direction());
    float distance = MaxDist * iInvScale;
    bool hit = iModel->IntersectRay(modRay, distance, StopAtFirstHit, ignoreFlags);
    if (hit)
    {
        distance *= iScale;
        MaxDist = distance;
    }
    return hit;
}

bool GameObjectModel::UpdatePosition()
{
    if (!iModel)
        return false;

    ModelList::const_iterator it = model_list.find(owner->GetDisplayId());
    if (it == model_list.end())
        return false;

    G3D::AABox mdl_box(it->second.bound);
    // ignore models with no bounds
    if (mdl_box == G3D::AABox::zero())
    {
        LOG_ERROR << fmt::format("Entity model {} has zero bounds, loading skipped", it->second.name.c_str());
        return false;
    }

    iPos = owner->GetPosition();

    G3D::Matrix3 iRotation = G3D::Matrix3::fromEulerAnglesZYX(owner->GetOrientation(), 0, 0);
    iInvRot = iRotation.inverse();
    // transform bounding box:
    mdl_box = AABox(mdl_box.low() * iScale, mdl_box.high() * iScale);
    AABox rotated_bounds;
    for (int i = 0; i < 8; ++i)
        rotated_bounds.merge(iRotation * mdl_box.corner(i));

    iBound = rotated_bounds + iPos;

    return true;
}

}
