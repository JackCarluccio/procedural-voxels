#pragma once

#include "world/chunk.h"
#include "world/block.h"

#include <array>

namespace voxels::world {

    class ChunkRegion {
    public:
        explicit ChunkRegion(std::array<std::array<Chunk*, 3>, 3> chunks);

        Block GetBlock(int x, int y, int z) const noexcept;
        void SetBlock(int x, int y, int z, Block block) noexcept;

    private:
        std::array<std::array<Chunk*, 3>, 3> chunks_;
    
    };

}
