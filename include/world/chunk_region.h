#pragma once

#include "world/chunk.h"
#include "world/block.h"

#include <array>

namespace voxels::world {

    class ChunkRegion {
    public:
        explicit ChunkRegion(std::array<std::array<Chunk*, 3>, 3> chunks)
            : chunks_(std::move(chunks)) {}

        Block GetBlock(int x, int y, int z) const noexcept {
            int chunk_x = (x + CHUNK_WIDTH) >> 4;
            int chunk_z = (z + CHUNK_WIDTH) >> 4;
            const Chunk* chunk = chunks_[chunk_x][chunk_z];
            return chunk->GetBlock(x & (CHUNK_WIDTH - 1), y, z & (CHUNK_WIDTH - 1));
        }

        void SetBlock(int x, int y, int z, Block block) noexcept {
            int chunk_x = (x + CHUNK_WIDTH) >> 4;
            int chunk_z = (z + CHUNK_WIDTH) >> 4;
            Chunk* chunk = chunks_[chunk_x][chunk_z];
            chunk->SetBlock(x & (CHUNK_WIDTH - 1), y, z & (CHUNK_WIDTH - 1), block);
        }

    private:
        std::array<std::array<Chunk*, 3>, 3> chunks_;
    
    };

}
