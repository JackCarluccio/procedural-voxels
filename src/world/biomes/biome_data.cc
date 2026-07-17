#include "world/biomes/biome_data.h"

namespace voxels::world::biome {

    BiomeData biome_data[] {
        { Block::Grass,     Block::Dirt },          // Plains
        { Block::Sand,      Block::SandStone },     // Desert
        { Block::Snow,      Block::Dirt },          // Tundra
        { Block::Stone,     Block::Stone },         // Mountains
    };
    
}
