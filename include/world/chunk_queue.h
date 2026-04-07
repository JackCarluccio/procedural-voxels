#ifndef VOXELS_WORLD_CHUNK_QUEUE_H_
#define VOXELS_WORLD_CHUNK_QUEUE_H_

#include "util/ivec2_hash.h"

#include <glm/vec2.hpp>

#include <unordered_set>

namespace voxels::world {

class ChunkQueue {
public:
    explicit ChunkQueue() = default;
    ~ChunkQueue() = default;

    ChunkQueue(const ChunkQueue&) = delete;
    ChunkQueue& operator=(const ChunkQueue&) = delete;
    ChunkQueue(ChunkQueue&&) = delete;
    ChunkQueue& operator=(ChunkQueue&&) = delete;

    glm::ivec2 Pop();
    void Push(const glm::ivec2& position) noexcept;

    bool IsEmpty() const noexcept {
        return queue_.empty();
    }

private:
    std::unordered_set<glm::ivec2, util::IVec2Hash> queue_;
};

}

#endif // VOXELS_WORLD_CHUNK_QUEUE_H_
