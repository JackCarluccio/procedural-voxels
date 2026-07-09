#include "world/chunk_queue.h"

namespace voxels::world {

    glm::ivec2 ChunkQueue::Pop() noexcept {
        auto it = queue_.begin();
        glm::ivec2 position = *it;
        queue_.erase(it);
        return position;
    }

    void ChunkQueue::Push(const glm::ivec2& position) noexcept {
        queue_.emplace(position);
    }

}
