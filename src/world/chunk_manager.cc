#include "world/chunk_manager.h"

#include <cmath>
#include <tuple>
#include <utility>

namespace voxels::world {

bool ChunkManager::HasChunk(const glm::ivec2& position) const noexcept {
    return chunks_.find(position) != chunks_.end();
}

bool ChunkManager::HasNeighbors(const glm::ivec2& position) const noexcept {
    return
        HasChunk(position + glm::ivec2(1, 0)) &&
        HasChunk(position + glm::ivec2(-1, 0)) &&
        HasChunk(position + glm::ivec2(0, 1)) &&
        HasChunk(position + glm::ivec2(0, -1));
}

Chunk& ChunkManager::GetChunk(const glm::ivec2& position) const {
    return *chunks_.at(position);
}

void ChunkManager::Update(const glm::vec3& player_position) {
    glm::ivec2 player_chunk_position = glm::ivec2(
        static_cast<int>(std::floor(player_position.x / Chunk::SIZE)),
        static_cast<int>(std::floor(player_position.z / Chunk::SIZE))
    );

    if (HasChunk(player_chunk_position)) {
        return;
    }

    GenerateChunk(player_chunk_position);
}

void ChunkManager::GenerateChunk(const glm::ivec2& position) {
    chunks_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(position),
        std::forward_as_tuple(std::make_unique<Chunk>(position))
    );
}
    
} // namespace voxels::world
