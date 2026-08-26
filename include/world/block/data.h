#pragma once

#include "world/block/block.h"
#include "world/block/face.h"

#include <cstdint>

namespace voxels::world::block {

    struct Data {
        uint8_t side_texture_index;
        uint8_t top_texture_index;
        uint8_t bottom_texture_index;
    };

    constexpr Data data[256] = {
        { 0, 0, 0 }, // Air
        { 0, 0, 0 }, // Stone
        { 1, 1, 1 }, // Dirt
        { 2, 3, 1 }, // Grass
        { 4, 5, 5 }, // Oak Log
        { 6, 6, 6 }, // Oak Leaves
        { 7, 7, 7 }, // Sand
        { 8, 9, 9 }, // SandStone
        { 10, 11, 1 } // Snow
    };

    constexpr uint8_t GetTextureIndex(const Data& data, Face face) noexcept {
        switch(face) {
            case Face::NegY: return data.bottom_texture_index;
            case Face::PosY: return data.top_texture_index;
            default: return data.side_texture_index;
        }
    }

    constexpr uint8_t GetTextureIndex(Block block, Face face) noexcept {
        return GetTextureIndex(data[static_cast<int>(block)], face);
    }

}
