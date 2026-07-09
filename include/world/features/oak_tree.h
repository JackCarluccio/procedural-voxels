#pragma once

#include "world/chunk_region.h"

namespace voxels::world::features {

    void BuildOakTree(ChunkRegion& chunk_region, int x, int y, int z) noexcept;
    bool CanBuildOakTree(const ChunkRegion& chunk_region, int x, int y, int z) noexcept;

}
