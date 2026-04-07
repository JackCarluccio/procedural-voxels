#include "world/chunk_manager.h"
#include "world/block.h"

#include <cmath>
#include <tuple>
#include <utility>

namespace voxels::world {

ChunkManager::ChunkManager()
    : chunk_generator_(std::make_unique<ChunkGenerator>()),
      chunk_mesher_(std::make_unique<ChunkMesher>()),
      chunk_queue_(std::make_unique<ChunkQueue>()) {}

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

void ChunkManager::Update(const glm::vec3& player_position) noexcept {
    glm::ivec2 player_chunk_position = glm::ivec2(
        static_cast<int>(std::floor(player_position.x / Chunk::SIZE)),
        static_cast<int>(std::floor(player_position.z / Chunk::SIZE))
    );

    static constexpr int RENDER_DISTANCE = 3;
    for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
        for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
            glm::ivec2 neighbor_position = player_chunk_position + glm::ivec2(x, z);
            if (!HasChunk(neighbor_position)) {
                chunk_queue_->Push(neighbor_position);
            }
        }
    }

    if (chunk_queue_->IsEmpty()) {
        return;
    }

    glm::ivec2 position = chunk_queue_->Pop();
    GenerateChunk(position);
}

void ChunkManager::GenerateChunk(const glm::ivec2& position) {
    auto [it, inserted] = chunks_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(position),
        std::forward_as_tuple(std::make_unique<Chunk>(position))
    );

    if (!inserted) {
        return;
    }

    Chunk& chunk = *it->second;
    chunk_generator_->Generate(chunk);
    chunk.SetMesh(chunk_mesher_->MeshChunk(chunk));
}
    
} // namespace voxels::world
