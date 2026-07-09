#include "world/features/oak_tree.h"

#include <cmath>

namespace voxels::world::features {

    void BuildOakTree(ChunkRegion& chunk_region, int x, int y, int z) noexcept {
        // Build trunk
        for (int i = 0; i < 5; i++) {
            chunk_region.SetBlock(x, y + i, z, Block::OakLog);
        }

        // Build leaves
        for (int dy = 0; dy < 3; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                for (int dz = -2; dz <= 2; dz++) {
                    if (std::abs(dx) + std::abs(dz) + dy <= 3) {
                        chunk_region.SetBlock(x + dx, y + 4 + dy, z + dz, Block::OakLeaves);
                    }
                }
            }
        }
    }

    bool CanBuildOakTree(const ChunkRegion& chunk_region, int x, int y, int z) noexcept {
        // Make sure trunk can fit
        for (int i = 0; i < 5; i++) {
            if (chunk_region.GetBlock(x, y + i, z) != Block::Air) {
                return false;
            }
        }

        for (int dy = 0; dy < 3; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                for (int dz = -2; dz <= 2; dz++) {
                    if (std::abs(dx) + std::abs(dz) + dy <= 3) {
                        if (chunk_region.GetBlock(x + dx, y + 4 + dy, z + dz) != Block::Air) {
                            return false;
                        }
                    }
                }
            }
        }
        
        return true;
    }

}
