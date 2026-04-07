#include "world/chunk_generator.h"

#include "world/block.h"

#include <cmath>

namespace voxels::world {

void ChunkGenerator::Generate(Chunk& chunk) {
    static constexpr int AMPLITUDE = 16;
    static constexpr int OFFSET = 16;
    static constexpr float FREQUENCY = 0.1f;

    int chunkWorldX = chunk.GetPosition().x * Chunk::SIZE;
    int chunkWorldZ = chunk.GetPosition().y * Chunk::SIZE;

    for (int x = 0; x < Chunk::SIZE; x++) {
        for (int z = 0; z < Chunk::SIZE; z++) {
            int terrainHeight = static_cast<int>(
                (
                    sin((x + chunkWorldX) * FREQUENCY) +
                    cos((z + chunkWorldZ) * FREQUENCY)
                ) * AMPLITUDE + OFFSET
            );

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
