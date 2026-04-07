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

    void Generate(Chunk& chunk);
};

}

#endif // VOXELS_WORLD_CHUNK_GENERATOR_H
