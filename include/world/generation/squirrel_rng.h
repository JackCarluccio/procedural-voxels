#pragma once

#include "world/generation/hash.h"

#include <cstdint>
#include <limits>

namespace voxels::world::generation {

    class SquirrelRNG {
    public:
        typedef uint32_t result_type;

        explicit SquirrelRNG(uint32_t seed) noexcept : seed_(seed), position_(0) {}

        constexpr static result_type min() noexcept { return 0; }
        constexpr static result_type max() noexcept { return std::numeric_limits<result_type>::max(); }
        result_type operator()() noexcept { return hash::Hash(position_++, seed_); }

    private:
        uint32_t seed_;
        uint32_t position_;
    };

}
