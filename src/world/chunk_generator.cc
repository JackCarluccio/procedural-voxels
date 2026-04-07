#include "world/chunk_generator.h"

#include "world/block.h"
#include "world/generation/perlin_noise_2d.h"
#include "world/helper.h"

#include <cmath>

namespace voxels::world {

namespace {

constexpr float AMPLITUDE = 32.0f;
constexpr int BASE_HEIGHT = 36.0f;
generation::PerlinNoise2d continentalnessSampler(0.025f, 3, 0.5f, 2.0f);

void GenerateHeightMap(int* heightMap, int chunkWorldX, int chunkWorldZ) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int height = static_cast<int>(
                continentalnessSampler.Sample(chunkWorldX + x, chunkWorldZ + z) * AMPLITUDE
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
