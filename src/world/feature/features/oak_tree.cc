#include "world/feature/features/oak_tree.h"

#include "world/generation/hash.h"

#include <cmath>

namespace voxels::world::feature {

    namespace {

        int GetTrunkHeight(int x, int z) noexcept {
            return static_cast<int>(generation::hash::FeatureHash(x, z) % 4 + 2);
        }

    }

    bool OakTree::CanGenerate(const chunk::Region& region, int x, int y, int z) const noexcept {
        int height = GetTrunkHeight(x, z);

        // All blocks in the trunk must be air
        for (int i = 0; i < height; i++) {
            if (region.GetBlock(x, y + i, z) != Block::Air) {
                return false;
            }
        }

        // All blocks in a 3x2x3 volume above the trunk must be air
        for (int dy = 0; dy < 2; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dz = -1; dz <= 1; dz++) {
                    if (region.GetBlock(x + dx, y + height + dy, z + dz) != Block::Air) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }

    void OakTree::Generate(chunk::Region& region, int x, int y, int z) const noexcept {
        int height = GetTrunkHeight(x, z);

        // Build trunk
        for (int i = 0; i < height; i++) {
            region.SetBlock(x, y + i, z, Block::OakLog);
        }

        // Build leaves
        for (int dy = 0; dy < 3; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                for (int dz = -2; dz <= 2; dz++) {
                    if (region.GetBlock(x + dx, y + height + dy, z + dz) != Block::Air) {
                        continue;
                    }

                    if (std::abs(dx) + std::abs(dz) + dy <= 3) {
                        region.SetBlock(x + dx, y + height + dy, z + dz, Block::OakLeaves);
                    }
                }
            }
        }
    }

}
