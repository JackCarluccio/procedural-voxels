#include "world/chunk_generator.h"

#include "world/block.h"
#include "world/generation/linear_spline.h"
#include "world/generation/perlin_noise_2d.h"
#include "world/helper.h"

#include <cmath>
#include <iostream>

namespace voxels::world {

namespace {

constexpr int BASE_HEIGHT = 4.0f;
constexpr float CONTINENTALNESS_AMPLITUDE = 240.0f;
generation::PerlinNoise2d continentalnessNoiseSampler(0.0125f / 4.0f, 5, 0.5f, 2.0f);
generation::LinearSpline continentalnessSpline({
    {-1.0f, 0.0f},
    {-0.15f, 0.4f},
    {-0.1f, 0.4f},
    {0.1f, 0.6f},
    {0.15f, 0.6f},
    {1.0f, 1.0f},
});

void GenerateHeightMap(int* heightMap, int chunkWorldX, int chunkWorldZ) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            float continentalnessNoise = continentalnessNoiseSampler.Sample(chunkWorldX + x, chunkWorldZ + z);
            int height = static_cast<int>(
                continentalnessSpline.GetValue(continentalnessNoise) * CONTINENTALNESS_AMPLITUDE
            ) + BASE_HEIGHT;

            heightMap[z + x * CHUNK_SIZE] = height;
        }
    }
}

} // namespace

void ChunkGenerator::Generate(Chunk& chunk) {
    int chunkWorldX = chunk.GetPosition().x * CHUNK_SIZE;
    int chunkWorldZ = chunk.GetPosition().y * CHUNK_SIZE;

    int heightMap[BLOCKS_PER_CHUNK_SLICE];
    GenerateHeightMap(heightMap, chunkWorldX, chunkWorldZ);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int terrainHeight = heightMap[z + x * CHUNK_SIZE];

            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                Block block;
                if (y >= terrainHeight) {
                    block = Block::Air;
                } else if (y == terrainHeight - 1) {
                    block = Block::Grass;
                } else if (y >= terrainHeight - 4) {
                    block = Block::Dirt;
                } else {
                    block = Block::Stone;
                }

                chunk.SetBlock(x, y, z, block);
            }
        }
    }
}

} // namespace voxels::world
