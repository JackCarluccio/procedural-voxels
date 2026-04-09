#include "world/chunk_generator.h"

#include "world/block.h"
#include "world/generation/linear_spline.h"
#include "world/generation/perlin_noise_2d.h"
#include "world/helper.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace voxels::world {

namespace {

constexpr int BASE_HEIGHT = 4.0f;
constexpr float CONTINENTALNESS_AMPLITUDE = 240.0f;
generation::PerlinNoise2d continentalness_noise_sampler(0.0125f / 4.0f, 5, 0.5f, 2.0f);
generation::LinearSpline continentalness_spline({
    {-1.00f, 0.00f},
    {-0.15f, 0.40f},
    {-0.10f, 0.40f},
    {+0.10f, 0.60f},
    {+0.15f, 0.60f},
    {+1.00f, 1.00f},
});

void GenerateHeightMap(const Chunk& chunk, int* height_map) {
    int chunk_world_x = chunk.GetPosition().x * CHUNK_SIZE;
    int chunk_world_z = chunk.GetPosition().y * CHUNK_SIZE;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float continentalness_noise = continentalness_noise_sampler.Sample(chunk_world_x + x, chunk_world_z + z);
            int height = static_cast<int>(
                continentalness_spline.GetValue(continentalness_noise) * CONTINENTALNESS_AMPLITUDE
            ) + BASE_HEIGHT;

            height_map[z + x * CHUNK_SIZE] = height;
        }
    }
}

void FillChunk(Chunk& chunk, int* height_map) {
    // The layout of blocks in memory is [y][x][z], so we can memset entire horizontal slices at a time
    // This allows us to vectorize the filling of all blocks below min_height and all above max_height
    int min_height = *std::min_element(height_map, height_map + BLOCKS_PER_CHUNK_SLICE);
    std::memset(chunk.GetBlocksPointer(), static_cast<int>(Block::Stone), BLOCKS_PER_CHUNK_SLICE * min_height);

    int max_height = *std::max_element(height_map, height_map + BLOCKS_PER_CHUNK_SLICE);
    std::memset(
        chunk.GetBlocksPointer() + max_height * BLOCKS_PER_CHUNK_SLICE,
        static_cast<int>(Block::Air),
        (CHUNK_HEIGHT - max_height) * BLOCKS_PER_CHUNK_SLICE
    );

    // Fill blocks between min_height and max_height according to the height map
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int terrainHeight = height_map[z + x * CHUNK_SIZE];

            chunk.SetBlock(x, terrainHeight - 1, z, Block::Grass);
            chunk.SetBlock(x, terrainHeight - 2, z, Block::Dirt);
            chunk.SetBlock(x, terrainHeight - 3, z, Block::Dirt);
            chunk.SetBlock(x, terrainHeight - 4, z, Block::Dirt);

            // Fill in stone from min_height to terrainHeight - 4
            for (int y = min_height; y < terrainHeight - 4; y++) {
                chunk.SetBlock(x, y, z, Block::Stone);
            }

            // Fill in air from terrainHeight to max_height
            for (int y = terrainHeight; y < max_height; y++) {
                chunk.SetBlock(x, y, z, Block::Air);
            }
        }
    }
}

} // namespace

void ChunkGenerator::Generate(Chunk& chunk) {
    int height_map[BLOCKS_PER_CHUNK_SLICE];
    GenerateHeightMap(chunk, height_map);
    FillChunk(chunk, height_map);
}

} // namespace voxels::world
