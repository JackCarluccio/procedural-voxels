#include "world/feature/features/mineral_pile.h"

#include "world/block/block.h"
#include "world/generation/hash.h"

#include <algorithm>
#include <array>
#include <vector>

namespace voxels::world::feature {

    namespace {

        constexpr std::array<block::Block, 5> mineral_blocks = {
            block::Block::Gravel,
            block::Block::Tuff,
            block::Block::Andesite,
            block::Block::Diorite,
            block::Block::Granite
        };

        block::Block GetRandomMineral(int x, int y, int z) noexcept {
            return mineral_blocks[generation::hash::FeatureHash(x, y, z) % mineral_blocks.size()];
        }

        int GetBlockCount(int x, int z) noexcept {
            return 3 + static_cast<int>(generation::hash::FeatureHash(x, z) % 30);
        }

        struct Location {
            int x, y, z;
        };

        bool IsValidLocation(const chunk::Region& region, int x, int y, int z) noexcept {
            return region.IsInBounds(x, y, z) &&
                region.GetBlock(x, y, z) == block::Block::Air &&
                y > 0 && region.GetBlock(x, y - 1, z) != block::Block::Air;
        }

        Location TakeRandomLocation(std::vector<Location>& locations, int x, int z, int blocks) noexcept {
            int index = generation::hash::FeatureHash(x, z, blocks) % locations.size();
            Location location = locations[index];

            // Order is irrelevant, so swap this element with the last one and pop back for O(1) removal
            std::swap(locations[index], locations.back());
            locations.pop_back();

            return location;
        }

        void AddNeighboringLocations(std::vector<Location>& locations, const Location& location) noexcept {
            locations.push_back({location.x + 1, location.y, location.z});
            locations.push_back({location.x - 1, location.y, location.z});
            locations.push_back({location.x, location.y + 1, location.z});
            // locations.push_back({location.x, location.y - 1, location.z});
            locations.push_back({location.x, location.y, location.z + 1});
            locations.push_back({location.x, location.y, location.z - 1});
        }

    }

    bool MineralPile::CanGenerate(const chunk::Region& region, int x, int y, int z) const noexcept {
        return IsValidLocation(region, x, y, z);
    }

    void MineralPile::Generate(chunk::Region& region, int x, int y, int z) const noexcept {
        int blocks = GetBlockCount(x, z);
        std::vector<Location> locations;
        locations.reserve(blocks * 5);
        locations.push_back({x, y, z});

        while (blocks > 0 && !locations.empty()) {
            Location location = TakeRandomLocation(locations, x, z, blocks);
            if (!IsValidLocation(region, location.x, location.y, location.z)) {
                continue;
            }

            block::Block mineral = GetRandomMineral(location.x, location.y, location.z);
            region.SetBlock(location.x, location.y, location.z, mineral);

            blocks--;
            AddNeighboringLocations(locations, location);
        }
    }

}
