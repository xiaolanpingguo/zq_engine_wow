#pragma once


#include "../grids/cell.h"
#include "../collision/dynamic_tree.h"
#include "../grids/grid_defines.h"
#include "../grids/grid_ref_manager.h"
#include "map_ref_manager.h"
#include "baselib/base_code/mpsc_queue.h"
#include "../common/common_defines.h"
#include "spawn_data.h"
#include "../common/timer.hpp"
#include <bitset>
#include <list>
#include <memory>
#include <mutex>
#include <optional>

namespace zq{

#define DEFAULT_VISIBILITY_DISTANCE 90.0f                   // default visible distance, 90 yards on continents
#define DEFAULT_VISIBILITY_INSTANCE 170.0f                  // default visible distance in instances, 170 yards
#define DEFAULT_VISIBILITY_BGARENAS 533.0f                  // default visible distance in BG/Arenas, roughly 533 yards

class Battleground;
class BattlegroundMap;
class CreatureGroup;
class GameObjectModel;
class Group;
class InstanceMap;
class InstanceSave;
class InstanceScript;
class MapInstanced;
class Object;
class Player;
class TempSummon;
class Transport;
class Unit;
class WorldObject;
class WorldPacket;
struct MapDifficulty;
struct MapEntry;
struct Position;
struct ScriptAction;
struct ScriptInfo;
struct SummonPropertiesEntry;
enum Difficulty : uint8;
namespace Trinity { struct ObjectUpdater; }
namespace VMAP { enum class ModelIgnoreFlags : uint32; }

struct ScriptAction
{
    uint64 sourceGUID;
    uint64 targetGUID;
    uint64 ownerGUID;                                   ///> owner of source if source is item
    ScriptInfo const* script;                               ///> pointer to static script data
};

/// Represents a map magic value of 4 bytes (used in versions)
union u_map_magic
{
    char asChar[4]; ///> Non-null terminated string
    uint32 asUInt;  ///> uint32 representation
};

// ******************************************
// Map file format defines
// ******************************************
struct map_fileheader
{
    u_map_magic mapMagic;
    u_map_magic versionMagic;
    u_map_magic buildMagic;
    uint32 areaMapOffset;
    uint32 areaMapSize;
    uint32 heightMapOffset;
    uint32 heightMapSize;
    uint32 liquidMapOffset;
    uint32 liquidMapSize;
    uint32 holesOffset;
    uint32 holesSize;
};

#define MAP_AREA_NO_AREA      0x0001

struct map_areaHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 gridArea;
};

#define MAP_HEIGHT_NO_HEIGHT            0x0001
#define MAP_HEIGHT_AS_INT16             0x0002
#define MAP_HEIGHT_AS_INT8              0x0004
#define MAP_HEIGHT_HAS_FLIGHT_BOUNDS    0x0008

struct map_heightHeader
{
    uint32 fourcc;
    uint32 flags;
    float  gridHeight;
    float  gridMaxHeight;
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

struct map_liquidHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 liquidType;
    uint8  offsetX;
    uint8  offsetY;
    uint8  width;
    uint8  height;
    float  liquidLevel;
};

enum ZLiquidStatus : uint32
{
    LIQUID_MAP_NO_WATER     = 0x00000000,
    LIQUID_MAP_ABOVE_WATER  = 0x00000001,
    LIQUID_MAP_WATER_WALK   = 0x00000002,
    LIQUID_MAP_IN_WATER     = 0x00000004,
    LIQUID_MAP_UNDER_WATER  = 0x00000008
};

#define MAP_LIQUID_STATUS_SWIMMING (LIQUID_MAP_IN_WATER | LIQUID_MAP_UNDER_WATER)
#define MAP_LIQUID_STATUS_IN_CONTACT (MAP_LIQUID_STATUS_SWIMMING | LIQUID_MAP_WATER_WALK)

#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_WATER       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_MAGMA       0x04
#define MAP_LIQUID_TYPE_SLIME       0x08

#define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME)

#define MAP_LIQUID_TYPE_DARK_WATER  0x10
#define MAP_LIQUID_TYPE_WMO_WATER   0x20

struct LiquidData
{
    uint32 type_flags;
    uint32 entry;
    float  level;
    float  depth_level;
};

struct PositionFullTerrainStatus
{
    struct AreaInfo
    {
        AreaInfo(int32 _adtId, int32 _rootId, int32 _groupId, uint32 _flags) : adtId(_adtId), rootId(_rootId), groupId(_groupId), mogpFlags(_flags) { }
        int32 const adtId;
        int32 const rootId;
        int32 const groupId;
        uint32 const mogpFlags;
    };

    PositionFullTerrainStatus() : areaId(0), floorZ(0.0f), outdoors(true) { }
    uint32 areaId;
    float floorZ;
    bool outdoors;
    ZLiquidStatus liquidStatus;
    std::optional<AreaInfo> areaInfo;
	std::optional<LiquidData> liquidInfo;
};

class GridMap
{
    uint32  _flags;
    union{
        float* m_V9;
        uint16* m_uint16_V9;
        uint8* m_uint8_V9;
    };
    union{
        float* m_V8;
        uint16* m_uint16_V8;
        uint8* m_uint8_V8;
    };
    int16* _maxHeight;
    int16* _minHeight;
    // Height level data
    float _gridHeight;
    float _gridIntHeightMultiplier;

    // Area data
    uint16* _areaMap;

    // Liquid data
    float _liquidLevel;
    uint16* _liquidEntry;
    uint8* _liquidFlags;
    float* _liquidMap;
    uint16 _gridArea;
    uint16 _liquidType;
    uint8 _liquidOffX;
    uint8 _liquidOffY;
    uint8 _liquidWidth;
    uint8 _liquidHeight;

    uint16* _holes;

    bool loadAreaData(FILE* in, uint32 offset, uint32 size);
    bool loadHeightData(FILE* in, uint32 offset, uint32 size);
    bool loadLiquidData(FILE* in, uint32 offset, uint32 size);
    bool loadHolesData(FILE* in, uint32 offset, uint32 size);
    bool isHole(int row, int col) const;

    // Get height functions and pointers
    typedef float (GridMap::*GetHeightPtr) (float x, float y) const;
    GetHeightPtr _gridGetHeight;
    float getHeightFromFloat(float x, float y) const;
    float getHeightFromUint16(float x, float y) const;
    float getHeightFromUint8(float x, float y) const;
    float getHeightFromFlat(float x, float y) const;

public:
    GridMap();
    ~GridMap();
    bool loadData(char const* filename);
    void unloadData();

    uint16 getArea(float x, float y) const;
    inline float getHeight(float x, float y) const {return (this->*_gridGetHeight)(x, y);}
    float getMinHeight(float x, float y) const;
    float getLiquidLevel(float x, float y) const;
    ZLiquidStatus GetLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData* data = 0, float collisionHeight = 2.03128f); // DEFAULT_COLLISION_HEIGHT in Object.h
};

#pragma pack(push, 1)

enum LevelRequirementVsMode
{
    LEVELREQUIREMENT_HEROIC = 70
};

struct ZoneDynamicInfo
{
    ZoneDynamicInfo() : MusicId(0), WeatherId(0), WeatherGrade(0.0f),
        OverrideLightId(0), LightFadeInTime(0) { }

    uint32 MusicId;
    uint32 WeatherId;
    float WeatherGrade;
    uint32 OverrideLightId;
    uint32 LightFadeInTime;
};

#pragma pack(pop)

#define MAX_HEIGHT            100000.0f                     // can be use for find ground height at surface
#define INVALID_HEIGHT       -100000.0f                     // for check, must be equal to VMAP_INVALID_HEIGHT, real value for unknown height is VMAP_INVALID_HEIGHT_VALUE
#define MAX_FALL_DISTANCE     250000.0f                     // "unlimited fall" to find VMap ground if it is available, just larger than MAX_HEIGHT - INVALID_HEIGHT
#define DEFAULT_HEIGHT_SEARCH     50.0f                     // default search distance to find height at nearby locations
#define MIN_UNLOAD_DELAY      1                             // immediate unload
#define MAP_INVALID_ZONE      0xFFFFFFFF

struct RespawnInfo; // forward declaration
struct CompareRespawnInfo
{
    bool operator()(RespawnInfo const* a, RespawnInfo const* b) const;
};
typedef std::unordered_map<uint32 /*zoneId*/, ZoneDynamicInfo> ZoneDynamicInfoMap;
typedef std::unordered_map<uint32, RespawnInfo*> RespawnInfoMap;
struct RespawnInfo
{
    SpawnObjectType type;
    uint32 spawnId;
    uint32 entry;
    time_t respawnTime;
    uint32 gridId;
    uint32 zoneId;
};
inline bool CompareRespawnInfo::operator()(RespawnInfo const* a, RespawnInfo const* b) const
{
    if (a == b)
        return false;
    if (a->respawnTime != b->respawnTime)
        return (a->respawnTime > b->respawnTime);
    if (a->spawnId != b->spawnId)
        return a->spawnId < b->spawnId;
    ASSERT(a->type != b->type, "Duplicate respawn entry for spawnId (%u,%u) found!", a->type, a->spawnId);
    return a->type < b->type;
}

class Map// : public GridRefManager<NGridType>
{
	friend class MapReference;
public:
	Map(uint32 id, time_t, uint32 InstanceId, uint8 SpawnMode, Map* _parent = nullptr);
	virtual ~Map();

	virtual void Update(uint32) {};
	virtual void DelayedUpdate(uint32 diff) {};
	virtual void InitVisibilityDistance();
	virtual void UnloadAll();

	bool CanUnload(uint32 diff)
	{
		if (!m_unloadTimer)
			return false;

		if (m_unloadTimer <= diff)
			return true;

		m_unloadTimer -= diff;
		return false;
	}

	bool Instanceable() const { return isInstanceble_; };
	void Instanceable(bool v) { isInstanceble_ = v; };
	bool IsDungeon() const;
	bool IsNonRaidDungeon() const;
	bool IsRaid() const;
	bool IsRaidOrHeroicDungeon() const;
	bool IsHeroic() const;
	bool Is25ManRaid() const;
	bool IsBattleground() const;
	bool IsBattleArena() const;
	bool IsBattlegroundOrArena() const;

	uint32 GetId() const { return mapid_; }

protected:

	uint32 mapid_;

	float m_VisibleDistance;
	int32 m_VisibilityNotifyPeriod;
	uint32 m_unloadTimer;

	bool isInstanceble_{ false };
	//bool isDungeon_;
};


class InstanceMap : public Map
{
public:
	InstanceMap(uint32 id, time_t, uint32 InstanceId, uint8 SpawnMode, Map* _parent);
	~InstanceMap();

	virtual void Update(uint32) {};
	virtual void DelayedUpdate(uint32 diff) {};
	void InitVisibilityDistance() override;

private:
	bool m_resetAfterUnload;
	bool m_unloadWhenEmpty;
	InstanceScript* i_data;
	uint32 i_script_id;
};



}
