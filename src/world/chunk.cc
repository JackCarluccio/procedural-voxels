#include "world/chunk.h"

namespace voxels::world {

Chunk::Chunk(const glm::ivec2& position) : position_(position) {
    blocks_.fill(Block::Air);
}

} // namespace voxels::world
