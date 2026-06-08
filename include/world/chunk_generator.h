#ifndef VOXELS_WORLD_CHUNK_GENERATOR_H
#define VOXELS_WORLD_CHUNK_GENERATOR_H

#include "world/chunk.h"

namespace voxels::world {

class ChunkGenerator {
public:
    explicit ChunkGenerator() = default;
    ~ChunkGenerator() = default;

    ChunkGenerator(const ChunkGenerator&) = delete;
    ChunkGenerator& operator=(const ChunkGenerator&) = delete;
    ChunkGenerator(ChunkGenerator&&) = delete;
    ChunkGenerator& operator=(ChunkGenerator&&) = delete;

    void Shape(Chunk& chunk) const noexcept;
    void Decorate(Chunk& chunk) const noexcept;
};

}

#endif // VOXELS_WORLD_CHUNK_GENERATOR_H
