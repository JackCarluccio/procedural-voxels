#include "world/biome/biome_data.h"

#include "world/feature/features/cactus.h"
#include "world/feature/features/mineral_pile.h"
#include "world/feature/features/oak_tree.h"

#include <memory>

namespace voxels::world::biome {

    using Block = block::Block;

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
            .surface_block = Block::SnowyDirt,
            .subsurface_block = Block::Dirt,
            .feature_commands {}
        },
        { // Mountains
            .surface_block = Block::Stone,
            .subsurface_block = Block::Stone,
            .feature_commands {}
        },
    };

    void AddFeatureCommand(Biome biome, std::unique_ptr<feature::Feature>&& feature, float average_count) {
        biome_data[static_cast<int>(biome)].feature_commands.push_back({
            .feature = std::move(feature),
            .average_count = average_count,
        });
    }

    void InitBiomeData() {
        AddFeatureCommand(Biome::Plains, std::make_unique<feature::OakTree>(), 10.0f);

        AddFeatureCommand(Biome::Desert, std::make_unique<feature::Cactus>(), 3.0f);

        AddFeatureCommand(Biome::Mountains, std::make_unique<feature::MineralPile>(), 0.8f);
    }
    
}
