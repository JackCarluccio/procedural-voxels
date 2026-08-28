#pragma once

#include <glm/vec3.hpp>

namespace voxels::util {

    class AABB {
    public:
        glm::vec3 min = glm::vec3(0.0f);
        glm::vec3 max = glm::vec3(0.0f);

        AABB() = default;
        explicit AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    };

}
