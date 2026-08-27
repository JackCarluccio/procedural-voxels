#pragma once

#include "world/block/block.h"
#include "world/chunk/region.h"
#include "world/feature/feature.h"

namespace voxels::world::feature {

    class Tree : public Feature {
    public:
        Tree(block::Block log_block, block::Block leaves_block)
            : log_block_(log_block), leaves_block_(leaves_block) {}

        bool CanGenerate(const chunk::Region& region, int x, int y, int z) const noexcept override;
        void Generate(chunk::Region& region, int x, int y, int z) const noexcept override;

    private:
        const block::Block log_block_;
        const block::Block leaves_block_;

    };

}
