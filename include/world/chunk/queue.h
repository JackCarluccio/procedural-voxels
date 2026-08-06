#pragma once

#include <glm/vec2.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace voxels::world::chunk {

    class Queue {
    public:
        explicit Queue(int queue_radius) {
            SetQueueRadius(queue_radius);
        }

        int GetQueueRadius() const noexcept { return queue_radius_; }

        void SetQueueRadius(int radius) noexcept {
            assert(radius >= 0 && "Queue radius must be non-negative");
            queue_radius_ = radius;
            EnsureRadiusFits();
        }

        void Update(const glm::ivec2& chunk_position) noexcept;

        bool IsEmpty() const noexcept { return size_ == 0; }

        glm::ivec2 Pop() noexcept;

    private:
        int queue_radius_;
        glm::ivec2 last_chunk_position_{-123, 123};
        std::size_t size_;
        std::vector<uint64_t> queue_;

        // Ensures the queue has enough capacity to hold all chunks within the queue radius
        void EnsureRadiusFits() noexcept;

        // Encodes the x, y, and distance into a single 64-bit integer for faster heap operations
        uint64_t Encode(int x, int y, int distance) const noexcept;
        glm::ivec2 Decode(uint64_t encoded) const noexcept;
    };

}
