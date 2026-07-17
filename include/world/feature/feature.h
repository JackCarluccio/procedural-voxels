#pragma once

#include "world/chunk_region.h"

namespace voxels::world::feature {

    class Feature {
    public:
        virtual ~Feature() = default;

        virtual bool CanGenerate(const ChunkRegion& chunk_region, int x, int y, int z) const noexcept = 0;
        virtual void Generate(ChunkRegion& chunk_region, int x, int y, int z) const noexcept = 0;

    };

}
