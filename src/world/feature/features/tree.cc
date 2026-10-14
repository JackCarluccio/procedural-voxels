#include "world/feature/features/tree.h"

#include "world/generation/hash.h"

#include <cmath>

namespace voxels::world::feature {

    namespace {

        int GetTrunkHeight(const chunk::Chunk& chunk, int x, int z) noexcept {
            int hash = generation::hash::FeatureHash(x + chunk.GetWorldX(), z + chunk.GetWorldZ());
            return 2 + static_cast<int>(hash % 4);
        }

    }

    bool Tree::CanGenerate(const chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept {
        int height = GetTrunkHeight(chunk, x, z);

        // All blocks in the trunk must be air
        for (int i = 0; i < height; i++) {
            if (region.GetBlock(x, y + i, z) != block::Block::Air) {
                return false;
            }
        }

        // All blocks in a 3x2x3 volume above the trunk must be air
        for (int dy = 0; dy < 2; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dz = -1; dz <= 1; dz++) {
                    if (region.GetBlock(x + dx, y + height + dy, z + dz) != block::Block::Air) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }

    void Tree::Generate(chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept {
        int height = GetTrunkHeight(chunk, x, z);

        // Build trunk
        for (int i = 0; i < height; i++) {
            region.SetBlock(x, y + i, z, log_block_);
        }

        // Build leaves
        for (int dy = 0; dy < 3; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                for (int dz = -2; dz <= 2; dz++) {
                    if (region.GetBlock(x + dx, y + height + dy, z + dz) != block::Block::Air) {
                        continue;
                    }

                    if (std::abs(dx) + std::abs(dz) + dy <= 3) {
                        region.SetBlock(x + dx, y + height + dy, z + dz, leaves_block_);
                    }
                }
            }
        }
    }

}
