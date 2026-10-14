#include "world/feature/features/cactus.h"

#include "world/block/block.h"
#include "world/generation/hash.h"

namespace voxels::world::feature {

    namespace {

        int GetCactusHeight(const chunk::Chunk& chunk, int x, int z) noexcept {
            uint32_t hash = generation::hash::FeatureHash(x + chunk.GetWorldX(), z + chunk.GetWorldZ());
            return 1 + static_cast<int>(hash % 4);
        }

        bool CanPlaceCactusBlock(const chunk::Region& region, int x, int y, int z) noexcept {
            return region.IsInBounds(x, y, z)
                && region.GetBlock(x, y, z) == block::Block::Air
                && region.GetBlock(x + 1, y, z) == block::Block::Air
                && region.GetBlock(x - 1, y, z) == block::Block::Air
                && region.GetBlock(x, y, z + 1) == block::Block::Air
                && region.GetBlock(x, y, z - 1) == block::Block::Air;
        }

    }

    bool Cactus::CanGenerate(const chunk::Region& region, const chunk::Chunk& /*chunk*/, int x, int y, int z) const noexcept {
        return CanPlaceCactusBlock(region, x, y, z)
            && y > 0 && region.GetBlock(x, y - 1, z) == block::Block::Sand;
    }

    void Cactus::Generate(chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept {
        int height = GetCactusHeight(chunk, x, z);
        for (int i = 0; i < height; ++i) {
            if (!CanPlaceCactusBlock(region, x, y + i, z)) {
                break;
            }

            region.SetBlock(x, y + i, z, block::Block::Cactus);
        }
    }

}
