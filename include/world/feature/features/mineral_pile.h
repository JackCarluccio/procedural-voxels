#pragma once

#include "world/feature/feature.h"

namespace voxels::world::feature {

    class MineralPile : public Feature {
    public:
        bool CanGenerate(const chunk::Region& region, int x, int y, int z) const noexcept override;
        void Generate(chunk::Region& region, int x, int y, int z) const noexcept override;
    };  

}
