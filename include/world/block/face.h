#pragma once

#include <cstdint>

namespace voxels::world::block {

    enum class Face {
        NegZ = 0,
        PosZ = 1,
        NegX = 2,
        PosX = 3,
        NegY = 4,
        PosY = 5
    };

}
