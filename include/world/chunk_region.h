#ifndef VOXELS_WORLD_CHUNK_REGION_H_
#define VOXELS_WORLD_CHUNK_REGION_H_

#include "world/chunk.h"
#include "world/block.h"

#include <array>

namespace voxels::world {

class ChunkRegion {
public:
    explicit ChunkRegion(std::array<std::array<Chunk*, 3>, 3> chunks);
    ~ChunkRegion() = default;

    ChunkRegion(const ChunkRegion&) = default;
    ChunkRegion& operator=(const ChunkRegion&) = default;
    ChunkRegion(ChunkRegion&&) = default;
    ChunkRegion& operator=(ChunkRegion&&) = default;

    Block GetBlock(int x, int y, int z) const noexcept;
    void SetBlock(int x, int y, int z, Block block) noexcept;

private:
    std::array<std::array<Chunk*, 3>, 3> chunks_;
};

}

#endif // VOXELS_WORLD_CHUNK_REGION_H_
