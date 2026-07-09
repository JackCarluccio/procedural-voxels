#pragma once

#include "util/ivec2_hash.h"

#include <unordered_set>

#include <glm/vec2.hpp>

namespace voxels::world {

    class ChunkQueue {
    public:
        ChunkQueue() = default;

        ChunkQueue(const ChunkQueue&) = delete;
        ChunkQueue& operator=(const ChunkQueue&) = delete;
        ChunkQueue(ChunkQueue&&) = delete;
        ChunkQueue& operator=(ChunkQueue&&) = delete;

        glm::ivec2 Pop() noexcept;
        void Push(const glm::ivec2& position) noexcept;

        bool IsEmpty() const noexcept { return queue_.empty(); }

    private:
        std::unordered_set<glm::ivec2, util::IVec2Hash> queue_;
    };

}
