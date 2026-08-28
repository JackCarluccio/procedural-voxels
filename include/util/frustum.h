#pragma once

#include "util/aabb.h"
#include "util/plane.h"

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace voxels::util {

    class Frustum {
    public:
        Plane planes[6];

        Frustum() = default;
        explicit Frustum(Plane planes[6]) : planes{planes[0], planes[1], planes[2], planes[3], planes[4], planes[5]} {}

        void NormalizePlanes() noexcept {
            for (auto& plane : planes) {
                plane.Normalize();
            }
        }

        bool ContainsAABB(const AABB& aabb) const noexcept {
            for (const auto& plane : planes) {
                glm::vec3 positiveVertex = aabb.min;
                if (plane.normal.x >= 0) positiveVertex.x = aabb.max.x;
                if (plane.normal.y >= 0) positiveVertex.y = aabb.max.y;
                if (plane.normal.z >= 0) positiveVertex.z = aabb.max.z;

                if (glm::dot(plane.normal, positiveVertex) + plane.distance < 0) {
                    return false;
                }
            }

            return true;
        }

    };

}
