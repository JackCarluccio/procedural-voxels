#pragma once

#include <glm/vec3.hpp>

namespace voxels::util {

    class AABB {
    public:
        glm::vec3 min = glm::vec3(0.0f);
        glm::vec3 max = glm::vec3(0.0f);

        constexpr AABB() noexcept = default;
        constexpr explicit AABB(const glm::vec3& min, const glm::vec3& max) noexcept : min(min), max(max) {}

    };

}
