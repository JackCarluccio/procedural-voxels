#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace voxels::graphics {
    class Camera;
}

namespace voxels::world::chunk {

    class Queue {
    public:
        explicit Queue(int queue_radius, const graphics::Camera* camera) noexcept : camera_(camera) {
            SetQueueRadius(queue_radius);
        }

        [[nodiscard]] int GetQueueRadius() const noexcept { return queue_radius_; }

        void SetQueueRadius(int radius) noexcept {
            assert(radius >= 0 && "Queue radius must be non-negative");
            queue_radius_ = radius;
            EnsureRadiusFits();
        }

        void Update(const glm::ivec2& chunk_position) noexcept;

        [[nodiscard]] bool IsEmpty() const noexcept { return size_ == 0; }

        glm::ivec2 Pop() noexcept;

    private:
        int queue_radius_;
        std::size_t size_;
        std::vector<uint64_t> queue_;

        const graphics::Camera* camera_;
        glm::mat4 last_camera_mat_;
        glm::ivec2 last_chunk_position_;

        [[nodiscard]] bool ShouldRebuild() noexcept;

        // Ensures the queue has enough capacity to hold all chunks within the queue radius
        void EnsureRadiusFits() noexcept;

        // Encodes the x, y, and distance into a single 64-bit integer for faster heap operations
        uint64_t Encode(int x, int y, int distance) const noexcept;
        glm::ivec2 Decode(uint64_t encoded) const noexcept;
    };

}
