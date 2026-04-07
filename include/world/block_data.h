#ifndef VOXELS_WORLD_BLOCK_DATA_H_
#define VOXELS_WORLD_BLOCK_DATA_H_

#include "world/block.h"
#include "world/face.h"

#include <cstdint>

namespace voxels::world {

struct BlockData {
    uint8_t side_texture_index;
    uint8_t top_texture_index;
    uint8_t bottom_texture_index;
};

constexpr BlockData block_data[256] = {
    {0, 0, 0}, // Air
    {0, 0, 0}, // Stone
    {1, 1, 1}, // Dirt
    {2, 3, 1}, // Grass
};

constexpr uint8_t GetTextureIndex(const BlockData& data, Face face) noexcept {
    switch(face) {
        case Face::NegY: return data.bottom_texture_index;
        case Face::PosY: return data.top_texture_index;
        default: return data.side_texture_index;
    }
}

constexpr uint8_t GetTextureIndex(Block block, Face face) noexcept {
    return GetTextureIndex(block_data[static_cast<int>(block)], face);
}

}

#endif // VOXELS_WORLD_BLOCK_DATA_H_
