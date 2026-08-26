#pragma once

#include "world/biome/biome.h"
#include "world/block/block.h"
#include "world/feature/feature_command.h"

#include <vector>

namespace voxels::world::biome {

    struct BiomeData {
        block::Block surface_block;
        block::Block subsurface_block;
        std::vector<feature::FeatureCommand> feature_commands;
    };
    extern BiomeData biome_data[];

    void InitBiomeData();

    inline block::Block GetSurfaceBlock(Biome biome) noexcept {
        return biome_data[static_cast<int>(biome)].surface_block;
    }

    inline block::Block GetSubsurfaceBlock(Biome biome) noexcept {
        return biome_data[static_cast<int>(biome)].subsurface_block;
    }
}
