#ifndef VOXELS_WORLD_CHUNK_GENERATOR_H
#define VOXELS_WORLD_CHUNK_GENERATOR_H

#include "world/chunk.h"
#include "world/chunk_region.h"
#include "world/helper.h"

namespace voxels::world {

class ChunkGenerator {
public:
    explicit ChunkGenerator() = default;
    ~ChunkGenerator() = default;

    ChunkGenerator(const ChunkGenerator&) = delete;
    ChunkGenerator& operator=(const ChunkGenerator&) = delete;
    ChunkGenerator(ChunkGenerator&&) = delete;
    ChunkGenerator& operator=(ChunkGenerator&&) = delete;

    void Shape(Chunk& chunk) noexcept;
    void Decorate(Chunk& chunk, ChunkRegion& chunk_region) const noexcept;

private:
    int height_map_[BLOCKS_PER_CHUNK_SLICE];
    float noise_map_[BLOCKS_PER_CHUNK_SLICE];

    void GenerateHeightMap(const Chunk& chunk) noexcept;
    void FillChunk(Chunk& chunk) noexcept;
};

}

#endif // VOXELS_WORLD_CHUNK_GENERATOR_H
