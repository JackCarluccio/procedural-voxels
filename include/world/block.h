#ifndef VOXELS_WORLD_BLOCK_H_
#define VOXELS_WORLD_BLOCK_H_

#include <cstdint>

namespace voxels::world {

enum class Block : uint8_t {
    Air = 0,
    Stone,
};

} // namespace voxels::world

#endif // VOXELS_WORLD_BLOCK_H_
