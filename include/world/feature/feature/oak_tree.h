#pragma once

#include "world/feature/feature.h"

namespace voxels::world::feature {

    class OakTree : public Feature {
    public:
        bool CanGenerate(const ChunkRegion& chunk_region, int x, int y, int z) const noexcept override;
        void Generate(ChunkRegion& chunk_region, int x, int y, int z) const noexcept override;
    };

}
