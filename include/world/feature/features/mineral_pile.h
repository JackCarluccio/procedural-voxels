#pragma once

#include "world/feature/feature.h"

namespace voxels::world::feature {

    class MineralPile : public Feature {
    public:
        [[nodiscard]] bool CanGenerate(const chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept override;
        void Generate(chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept override;
    };  

}
