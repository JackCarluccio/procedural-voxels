#pragma once

#include "world/chunk/chunk.h"
#include "world/chunk/region.h"

namespace voxels::world::feature {

    class Feature {
    public:
        virtual ~Feature() = default;

        [[nodiscard]] virtual bool CanGenerate(const chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept = 0;
        virtual void Generate(chunk::Region& region, const chunk::Chunk& chunk, int x, int y, int z) const noexcept = 0;

    };

}
