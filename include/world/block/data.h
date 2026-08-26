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

    inline Data data[256] = { 
        { 0, 0, 0 } // Air
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
