#include "world/chunk_generator.h"

#include "world/block.h"
#include "world/generation/perlin_noise_2d.h"

#include <cmath>

namespace voxels::world {

namespace {

constexpr int AMPLITUDE = 16;
constexpr int BASE_HEIGHT = 16;
constexpr float FREQUENCY = 0.1f;

void GenerateHeightMap(int* heightMap, int chunkWorldX, int chunkWorldZ) {
    for (int x = 0; x < Chunk::SIZE; x++) {
        for (int z = 0; z < Chunk::SIZE; z++) {
            int height = static_cast<int>(
                generation::PerlinNoise2d(
                    (x + chunkWorldX) * FREQUENCY,
                    (z + chunkWorldZ) * FREQUENCY
                ) * AMPLITUDE + BASE_HEIGHT
            );

            heightMap[z + x * Chunk::SIZE] = height;
        }
    }
}

} // namespace

void ChunkGenerator::Generate(Chunk& chunk) {
    int chunkWorldX = chunk.GetPosition().x * Chunk::SIZE;
    int chunkWorldZ = chunk.GetPosition().y * Chunk::SIZE;

    int heightMap[Chunk::BLOCKS_PER_SLICE];
    GenerateHeightMap(heightMap, chunkWorldX, chunkWorldZ);

    for (int x = 0; x < Chunk::SIZE; x++) {
        for (int z = 0; z < Chunk::SIZE; z++) {
            int terrainHeight = heightMap[z + x * Chunk::SIZE];

            for (int y = 0; y < Chunk::HEIGHT; y++) {
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
