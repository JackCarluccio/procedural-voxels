#pragma once

#include <glm/vec2.hpp>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace voxels::world {

    class ChunkManager;

    class ChunkQueue {
    public:
        explicit ChunkQueue(const ChunkManager& chunk_manager, int render_radius)
            : chunk_manager_(chunk_manager), render_radius_(render_radius) {
            assert(render_radius_ >= 0 && "ChunkQueue render radius must be non-negative");
        }

        int GetRenderRadius() const noexcept { return render_radius_; }

        void SetRenderRadius(int radius) noexcept {
            assert(radius >= 0 && "ChunkQueue render radius must be non-negative");
            render_radius_ = radius;
        }

        void Update(const glm::ivec2& chunk_position) noexcept;

        bool IsEmpty() const noexcept { return size_ == 0; }

        glm::ivec2 Pop() noexcept;

    private:
        const ChunkManager& chunk_manager_;
        int render_radius_;
        glm::ivec2 last_chunk_position_{-123, 123};
        std::size_t size_;
        std::vector<uint64_t> queue_;

        // Ensures the queue has enough capacity to hold all chunks within the render radius
        void EnsureRadiusFits() noexcept;

        // Encodes the x, y, and distance into a single 64-bit integer for faster heap operations
        uint64_t Encode(int x, int y, int distance) const noexcept;
        glm::ivec2 Decode(uint64_t encoded) const noexcept;
    };

}
