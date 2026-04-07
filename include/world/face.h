#ifndef VOXELS_WORLD_FACE_H_
#define VOXELS_WORLD_FACE_H_

#include <cstdint>

namespace voxels::world {

enum class Face : uint8_t {
    NegZ = 0,
    PosZ = 1,
    NegX = 2,
    PosX = 3,
    NegY = 4,
    PosY = 5
};

}

#endif // VOXELS_WORLD_FACE_H_
