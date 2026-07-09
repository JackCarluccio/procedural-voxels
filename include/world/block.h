#pragma once

#include <cstdint>

namespace voxels::world {

    enum class Block : uint8_t {
        Air = 0,
        Stone,
        Dirt,
        Grass,
        OakLog,
        OakLeaves,
    };

}