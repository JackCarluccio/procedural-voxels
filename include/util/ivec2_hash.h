#pragma once

#include <stddef.h>
#include <functional>

#include <glm/vec2.hpp>

namespace voxels::util {

    struct IVec2Hash {
        std::size_t operator()(const glm::ivec2& v) const noexcept {
            std::size_t h1 = std::hash<int>{}(v.x);
            std::size_t h2 = std::hash<int>{}(v.y);
            return h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        }
    };

}
