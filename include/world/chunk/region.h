#pragma once

#include "world/chunk/chunk.h"
#include "world/block/block.h"

#include <array>

namespace voxels::world::chunk {

    class Region {
    public:
        explicit Region(const std::array<std::array<Chunk*, 3>, 3>& chunks)
            : chunks_(chunks) {}

        block::Block GetBlock(int x, int y, int z) const noexcept {
            int chunk_x = (x + Chunk::WIDTH) >> 4;
            int chunk_z = (z + Chunk::WIDTH) >> 4;
            const Chunk* chunk = chunks_[chunk_x][chunk_z];
            return chunk->GetBlock(x & (Chunk::WIDTH - 1), y, z & (Chunk::WIDTH - 1));
        }

        void SetBlock(int x, int y, int z, block::Block block) noexcept {
            int chunk_x = (x + Chunk::WIDTH) >> 4;
            int chunk_z = (z + Chunk::WIDTH) >> 4;
            Chunk* chunk = chunks_[chunk_x][chunk_z];
            chunk->SetBlock(x & (Chunk::WIDTH - 1), y, z & (Chunk::WIDTH - 1), block);
        }

    private:
        std::array<std::array<Chunk*, 3>, 3> chunks_;
    
    };

}
