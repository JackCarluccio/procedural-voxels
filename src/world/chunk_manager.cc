#include "world/chunk_manager.h"

#include "world/block.h"
#include "world/helper.h"

#include <cmath>
#include <tuple>
#include <utility>

namespace voxels::world {

ChunkManager::ChunkManager()
    : chunk_generator_(std::make_unique<ChunkGenerator>()),
      chunk_mesher_(std::make_unique<ChunkMesher>()),
      chunk_queue_(std::make_unique<ChunkQueue>())
{}

void ChunkManager::Init() const noexcept {
    helper::Init();
    chunk_mesher_->Init();
}

bool ChunkManager::HasChunk(const glm::ivec2& position) const noexcept {
    return chunks_.find(position) != chunks_.end();
}

bool ChunkManager::HasCardinalNeighbors(const glm::ivec2& position) const noexcept {
    return
        HasChunk(position + CARDINAL_CHUNK_OFFSETS[0]) &&
        HasChunk(position + CARDINAL_CHUNK_OFFSETS[1]) &&
        HasChunk(position + CARDINAL_CHUNK_OFFSETS[2]) &&
        HasChunk(position + CARDINAL_CHUNK_OFFSETS[3]);
}

Chunk& ChunkManager::GetChunk(const glm::ivec2& position) const {
    return *chunks_.at(position);
}

void ChunkManager::Update(const glm::vec3& player_position) noexcept {
    glm::ivec2 player_chunk_position = glm::ivec2(
        static_cast<int>(std::floor(player_position.x / CHUNK_SIZE)),
        static_cast<int>(std::floor(player_position.z / CHUNK_SIZE))
    );

    static constexpr int RENDER_DISTANCE = 10;
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

std::array<const Chunk* const, 4> ChunkManager::GetCardinalNeighborPointers(const glm::ivec2& position) const {
    return {
        chunks_.at(position + CARDINAL_CHUNK_OFFSETS[0]).get(),
        chunks_.at(position + CARDINAL_CHUNK_OFFSETS[1]).get(),
        chunks_.at(position + CARDINAL_CHUNK_OFFSETS[2]).get(),
        chunks_.at(position + CARDINAL_CHUNK_OFFSETS[3]).get()
    };
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

    // This chunk may have all neighbors needed for meshing
    if (HasCardinalNeighbors(position)) {
        chunk.SetMesh(chunk_mesher_->MeshChunk(chunk, GetCardinalNeighborPointers(position)));
    }

    // Check if any of the neighboring chunks can be meshed now
    for (const auto& offset : CARDINAL_CHUNK_OFFSETS) {
        // Chunks can only be meshed if all cardinal neighbors exist
        glm::ivec2 neighbor_position = position + offset;
        if (!HasChunk(neighbor_position) || !HasCardinalNeighbors(neighbor_position)) {
            continue;
        }

        Chunk& neighbor_chunk = *chunks_.at(neighbor_position);
        if (neighbor_chunk.GetMesh() != nullptr) {
            continue;
        }

        neighbor_chunk.SetMesh(chunk_mesher_->MeshChunk(neighbor_chunk, GetCardinalNeighborPointers(neighbor_position)));
    }
}
    
} // namespace voxels::world
