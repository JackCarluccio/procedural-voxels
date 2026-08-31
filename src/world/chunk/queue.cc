#include "world/chunk/queue.h"

#include "graphics/camera.h"
#include "util/aabb.h"
#include "util/frustum.h"
#include "world/chunk/chunk.h"
#include "world/chunk/manager.h"

#include <algorithm>
#include <vector>
#include <functional>
#include <numbers>
#include <utility>

namespace voxels::world::chunk {

    void Queue::Update(const glm::ivec2& chunk_position) noexcept {
        if (!ShouldRebuild()) {
            return;
        }
        last_chunk_position_ = chunk_position;

        const util::Frustum frustum = camera_->GetFrustum();

        int index = 0;
        const int queue_radius_squared = queue_radius_ * queue_radius_;
        for (int x = -queue_radius_; x <= queue_radius_; x++) {
            for (int y = -queue_radius_; y <= queue_radius_; y++) {
                int distance = x * x + y * y;
                if (distance > queue_radius_squared) {
                    continue;
                }

                util::AABB chunk_aabb = Chunk::GetAABB(chunk_position + glm::ivec2(x, y));
                // Expand the AABB since we have two 2 rings of chunk states before chunks can mesh
                chunk_aabb.min.x -= 2.0f * Chunk::WIDTH;
                chunk_aabb.min.z -= 2.0f * Chunk::WIDTH;
                chunk_aabb.max.x += 2.0f * Chunk::WIDTH;
                chunk_aabb.max.z += 2.0f * Chunk::WIDTH;
                if (!frustum.ContainsAABB(chunk_aabb)) {
                    continue;
                }
                
                queue_[index] = Encode(x, y, distance);
                index++;
            }
        }

        size_ = index;
        std::make_heap(queue_.begin(), queue_.begin() + size_, std::greater<uint64_t>{});
    }

    glm::ivec2 Queue::Pop() noexcept {
        assert(size_ > 0 && "Cannot pop from an empty ChunkQueue");

        std::pop_heap(queue_.begin(), queue_.begin() + size_, std::greater<uint64_t>{});
        uint64_t encoded = queue_[size_ - 1];
        size_--;

        return Decode(encoded);
    }

    bool Queue::ShouldRebuild() noexcept {
        glm::mat4 camera_mat = camera_->GetViewProjectionMatrix();
        if (camera_mat != last_camera_mat_) {
            last_camera_mat_ = camera_mat;
            return true;
        }

        return false;
    }

    void Queue::EnsureRadiusFits() noexcept {
        float radius = static_cast<float>(queue_radius_);
        int upper_bound = static_cast<int>(std::numbers::pi * radius * radius + 2.0f * std::numbers::pi * radius) + 1;
        queue_.resize(upper_bound);
    }

    uint64_t Queue::Encode(int x, int y, int distance) const noexcept {
        uint64_t encoded = static_cast<uint64_t>(distance) << 32;
        encoded |= (static_cast<uint64_t>(x) & 0xFFFF) << 16;
        encoded |= (static_cast<uint64_t>(y) & 0xFFFF);
        return encoded;
    }

    glm::ivec2 Queue::Decode(uint64_t encoded) const noexcept {
        // Cast to int16_t so position is sign-extended correctly when converting to int
        int x = static_cast<int16_t>((encoded >> 16) & 0xFFFF);
        int y = static_cast<int16_t>(encoded & 0xFFFF);
        return glm::ivec2(x, y) + last_chunk_position_;
    }

}
