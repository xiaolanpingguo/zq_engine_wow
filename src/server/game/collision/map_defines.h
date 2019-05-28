#pragma once


#include "interface_header/base/platform.h"
#include "DetourNavMesh.h"
#include <stdio.h>

namespace zq{


static const uint32 MMAP_MAGIC = 0x4d4d4150; // 'MMAP'

// vmap_define
static const char VMAP_MAGIC[] = "VMAP_4.4";
static const char RAW_VMAP_MAGIC[] = "VMAP044";                // used in extracted vmap files with raw data
static const char GAMEOBJECT_MODELS[] = "GameObjectModels.dtree";

#define MMAP_VERSION 7
#define LIQUID_TILE_SIZE (533.333f / 128.f)


struct MmapTileHeader
{
    uint32 mmapMagic;
    uint32 dtVersion;
    uint32 mmapVersion;
    uint32 size;
    char usesLiquids;
    char padding[3];

    MmapTileHeader() : mmapMagic(MMAP_MAGIC), dtVersion(DT_NAVMESH_VERSION),
        mmapVersion(MMAP_VERSION), size(0), usesLiquids(true), padding() { }
};

// All padding fields must be handled and initialized to ensure mmaps_generator will produce binary-identical *.mmtile files
static_assert(sizeof(MmapTileHeader) == 20, "MmapTileHeader size is not correct, adjust the padding field size");
static_assert(sizeof(MmapTileHeader) == (sizeof(MmapTileHeader::mmapMagic) +
                                         sizeof(MmapTileHeader::dtVersion) +
                                         sizeof(MmapTileHeader::mmapVersion) +
                                         sizeof(MmapTileHeader::size) +
                                         sizeof(MmapTileHeader::usesLiquids) +
                                         sizeof(MmapTileHeader::padding)), "MmapTileHeader has uninitialized padding fields");

enum NavTerrain
{
    NAV_EMPTY   = 0x00,
    NAV_GROUND  = 0x01,
    NAV_MAGMA   = 0x02,
    NAV_SLIME   = 0x04,
    NAV_WATER   = 0x08,
    NAV_UNUSED1 = 0x10,
    NAV_UNUSED2 = 0x20,
    NAV_UNUSED3 = 0x40,
    NAV_UNUSED4 = 0x80
    // we only have 8 bits
};

bool readChunk(FILE* rf, char *dest, const char *compare, uint32 len);

}