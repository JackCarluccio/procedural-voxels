#include "world/chunk_manager.h"

#include "world/block.h"
#include "world/chunk_stage.h"
#include "world/helper.h"

#include <cmath>
#include <tuple>
#include <utility>

namespace voxels::world {

    ChunkManager::ChunkManager()
        : chunk_generator_(std::make_unique<ChunkGenerator>()),
        chunk_mesher_(std::make_unique<ChunkMesher>()),
        chunk_queue_(std::make_unique<ChunkQueue>(*this, 16))
    {}

    void ChunkManager::Init() const noexcept {
        chunk_generator_->Init();
    }

    void ChunkManager::Update(const glm::vec3& player_position) noexcept {
        glm::ivec2 player_chunk_position = glm::ivec2(
            static_cast<int>(std::floor(player_position.x / CHUNK_WIDTH)),
            static_cast<int>(std::floor(player_position.z / CHUNK_WIDTH))
        );

        chunk_queue_->Update(player_chunk_position);

        int total_operations = 0;
        while (total_operations < 8) {
            if (!chunks_to_mesh_.empty()) {
                Chunk* chunk = chunks_to_mesh_.back();
                chunks_to_mesh_.pop_back();
                MeshChunk(*chunk);
                total_operations++;
            } else if (!chunks_to_decorate_.empty()) {
                Chunk* chunk = chunks_to_decorate_.back();
                chunks_to_decorate_.pop_back();
                DecorateChunk(*chunk);
                total_operations++;
            } else if (!chunk_queue_->IsEmpty()) {
                glm::ivec2 position = chunk_queue_->Pop();
                GenerateChunk(position);
                total_operations++;
            } else {
                break;
            }
        }
    }

    ChunkRegion ChunkManager::GetChunkRegion(const glm::ivec2& position) noexcept {
        return ChunkRegion({{
            {{
                chunks_.find(position + glm::ivec2(-1, -1))->second.get(),
                chunks_.find(position + glm::ivec2(-1, 0 ))->second.get(),
                chunks_.find(position + glm::ivec2(-1, 1 ))->second.get(),
            }},
            {{
                chunks_.find(position + glm::ivec2( 0, -1))->second.get(), 
                chunks_.find(position                     )->second.get(),
                chunks_.find(position + glm::ivec2( 0, 1 ))->second.get(),
            }},
            {{
                chunks_.find(position + glm::ivec2( 1, -1))->second.get(),
                chunks_.find(position + glm::ivec2( 1, 0 ))->second.get(),
                chunks_.find(position + glm::ivec2( 1, 1 ))->second.get(),
            }},
        }});
    }

    ChunkRegion ChunkManager::GetChunkRegion(const Chunk& chunk) noexcept {
        return GetChunkRegion(chunk.GetPosition());
    }

    void ChunkManager::GenerateChunk(const glm::ivec2& position) noexcept {
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
        ChunkRegion chunk_region = GetChunkRegion(chunk);
        chunk_generator_->Decorate(chunk, chunk_region);
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
        ChunkRegion chunk_region = GetChunkRegion(chunk);
        chunk.SetMesh(chunk_mesher_->MeshChunk(chunk, chunk_region));
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
    
}
