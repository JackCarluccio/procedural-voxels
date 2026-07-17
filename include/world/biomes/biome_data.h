#pragma once

#include "world/biomes/biome.h"
#include "world/block.h"

namespace voxels::world::biome {

    struct BiomeData {
        Block surface_block;
        Block subsurface_block;
    };
    extern BiomeData biome_data[];

    inline Block GetSurfaceBlock(Biome biome) noexcept {
        return biome_data[static_cast<int>(biome)].surface_block;
    }

    inline Block GetSubsurfaceBlock(Biome biome) noexcept {
        return biome_data[static_cast<int>(biome)].subsurface_block;
    }
}
