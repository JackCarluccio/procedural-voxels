#include "world/biome/biome_data.h"

#include "world/feature/features/oak_tree.h"

#include <memory>

namespace voxels::world::biome {

    BiomeData biome_data[] {
        { // Plains
            .surface_block = Block::Grass,
            .subsurface_block = Block::Dirt,
            .feature_commands {}
        },
        { // Desert
            .surface_block = Block::Sand,
            .subsurface_block = Block::SandStone,
            .feature_commands {}
        },
        { // Tundra
            .surface_block = Block::Snow,
            .subsurface_block = Block::Dirt,
            .feature_commands {}
        },
        { // Mountains
            .surface_block = Block::Stone,
            .subsurface_block = Block::Stone,
            .feature_commands {}
        },
    };

    void InitBiomeData() {
        // Plains
        biome_data[static_cast<int>(Biome::Plains)].feature_commands.push_back({
            .feature = std::make_unique<feature::OakTree>(),
            .attempts = 10,
        });
    }
    
}
