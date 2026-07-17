#pragma once

#include "world/chunk.h"
#include "world/chunk_region.h"

namespace voxels::world {

    class ChunkGenerator {
    public:
        ChunkGenerator() = default;

        ChunkGenerator(const ChunkGenerator&) = delete;
        ChunkGenerator& operator=(const ChunkGenerator&) = delete;
        ChunkGenerator(ChunkGenerator&&) = delete;
        ChunkGenerator& operator=(ChunkGenerator&&) = delete;

        void Init();

        void Shape(Chunk& chunk) noexcept;
        void Decorate(Chunk& chunk, ChunkRegion& chunk_region) const noexcept;
    };

}
