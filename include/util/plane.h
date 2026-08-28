#pragma once

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace voxels::util {

    class Plane {
    public:
        glm::vec3 normal = glm::vec3(0.0f);
        float distance = 0.0f;

        Plane() = default;
        explicit Plane(const glm::vec3& normal, float distance) : normal(normal), distance(distance) {}

        void Normalize() noexcept {
            float length = glm::length(normal);
            if (length > 0.0f) {
                normal /= length;
                distance /= length;
            }
        }

    };

}
