#pragma once

#include "world/chunk/region.h"

namespace voxels::world::feature {

    class Feature {
    public:
        virtual ~Feature() = default;

        virtual bool CanGenerate(const chunk::Region& region, int x, int y, int z) const noexcept = 0;
        virtual void Generate(chunk::Region& region, int x, int y, int z) const noexcept = 0;

    };

}
