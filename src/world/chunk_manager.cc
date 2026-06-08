#include "world/chunk_manager.h"

#include "world/block.h"
#include "world/chunk_stage.h"
#include "world/helper.h"

#include <cmath>
#include <tuple>
#include <utility>
#include <iostream>
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
        HasChunk(position + CHUNK_CARDINAL_OFFSETS[0]) &&
        HasChunk(position + CHUNK_CARDINAL_OFFSETS[1]) &&
        HasChunk(position + CHUNK_CARDINAL_OFFSETS[2]) &&
        HasChunk(position + CHUNK_CARDINAL_OFFSETS[3]);
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

    if (!chunks_to_decorate_.empty()) {
        DecorateChunk(*chunks_to_decorate_.back());
        chunks_to_decorate_.pop_back();
    }

    if (!chunks_to_mesh_.empty()) {
        MeshChunk(*chunks_to_mesh_.back());
        chunks_to_mesh_.pop_back();
    }

    if (!chunk_queue_->IsEmpty()) {
        glm::ivec2 position = chunk_queue_->Pop();
        GenerateChunk(position);
    }
}

std::array<const Chunk* const, 4> ChunkManager::GetCardinalNeighborPointers(const glm::ivec2& position) const {
    return {
        chunks_.at(position + CHUNK_CARDINAL_OFFSETS[0]).get(),
        chunks_.at(position + CHUNK_CARDINAL_OFFSETS[1]).get(),
        chunks_.at(position + CHUNK_CARDINAL_OFFSETS[2]).get(),
        chunks_.at(position + CHUNK_CARDINAL_OFFSETS[3]).get()
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

    ShapeChunk(*it->second);
}

void ChunkManager::ShapeChunk(Chunk& chunk) noexcept {
    chunk_generator_->Shape(chunk);
    chunk.SetStage(ChunkStage::Shaped);

    if (IsDecoratable(chunk)) {
        chunks_to_decorate_.push_back(&chunk);
    }

    // Now that this chunk is shaped, some if its neighbors may be decoratable
    for (const auto& offset : CHUNK_OFFSETS) {
        auto it = chunks_.find(chunk.GetPosition() + offset);
        if (it != chunks_.end() && IsDecoratable(*it->second)) {
            chunks_to_decorate_.push_back(it->second.get());
        }
    }
}

void ChunkManager::DecorateChunk(Chunk& chunk) noexcept {
    chunk_generator_->Decorate(chunk);
    chunk.SetStage(ChunkStage::Decorated);

    if (IsMeshable(chunk)) {
        chunks_to_mesh_.push_back(&chunk);
    }

    // Now that this chunk is decorated, some if its neighbors may be meshable
    for (const auto& offset : CHUNK_OFFSETS) {
        auto it = chunks_.find(chunk.GetPosition() + offset);
        if (it == chunks_.end() || !IsMeshable(*it->second)) {
            continue;
        }

        chunks_to_mesh_.push_back(it->second.get());
    }
}

void ChunkManager::MeshChunk(Chunk& chunk) noexcept {
    chunk.SetMesh(chunk_mesher_->MeshChunk(chunk, GetCardinalNeighborPointers(chunk.GetPosition())));
    chunk.SetStage(ChunkStage::Meshed);
}

bool ChunkManager::IsDecoratable(const Chunk& chunk) const noexcept {
    if (chunk.GetStage() != ChunkStage::Shaped) {
        return false;
    }

    for (const auto& offset : CHUNK_OFFSETS) {
        auto other_chunk = chunks_.find(chunk.GetPosition() + offset);
        if (other_chunk == chunks_.end() || other_chunk->second->GetStage() < ChunkStage::Shaped) {
            return false;
        }
    }

    return true;
}

bool ChunkManager::IsMeshable(const Chunk& chunk) const noexcept {
    if (chunk.GetStage() != ChunkStage::Decorated) {
        return false;
    }

    for (const auto& offset : CHUNK_OFFSETS) {
        auto other_chunk = chunks_.find(chunk.GetPosition() + offset);
        if (other_chunk == chunks_.end() || other_chunk->second->GetStage() < ChunkStage::Decorated) {
            return false;
        }
    }

    return true;
}
    
} // namespace voxels::world
