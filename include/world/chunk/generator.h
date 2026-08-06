#pragma once

#include "world/chunk/chunk.h"
#include "world/chunk/region.h"

namespace voxels::world::chunk {

    class Generator {
    public:
        Generator() = default;

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;
        Generator(Generator&&) = delete;
        Generator& operator=(Generator&&) = delete;

        void Init();

        void Shape(Chunk& chunk) noexcept;
        void Decorate(Chunk& chunk, Region& region) const noexcept;
    };

}
