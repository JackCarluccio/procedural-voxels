#include "world/chunk/manager.h"

#include "world/block.h"
#include "world/chunk/stage.h"

#include <cmath>
#include <tuple>
#include <utility>

#include <glm/vec2.hpp>

namespace {

    constexpr glm::ivec2 CHUNK_OFFSETS[8] = {
        glm::ivec2( 1,  0),
        glm::ivec2( 1,  1),
        glm::ivec2( 0,  1),
        glm::ivec2(-1,  1),
        glm::ivec2(-1,  0),
        glm::ivec2(-1, -1),
        glm::ivec2( 0, -1),
        glm::ivec2( 1, -1),
    };

}

namespace voxels::world::chunk {

    Manager::Manager()
        : generator_(),
        mesher_(),
        queue_(16)
    {}

    void Manager::Init() noexcept {
        generator_.Init();
    }

    void Manager::Update(const glm::vec3& player_position) noexcept {
        glm::ivec2 player_chunk_position = glm::ivec2(
            static_cast<int>(std::floor(player_position.x / Chunk::WIDTH)),
            static_cast<int>(std::floor(player_position.z / Chunk::WIDTH))
        );

        queue_.Update(player_chunk_position);

        int operations = 0;
        while (operations < 8) {
            if (!chunks_to_mesh_.empty()) {
                Chunk* chunk = chunks_to_mesh_.back();
                chunks_to_mesh_.pop_back();
                operations++;
                MeshChunk(*chunk);
            } else if (!chunks_to_decorate_.empty()) {
                Chunk* chunk = chunks_to_decorate_.back();
                chunks_to_decorate_.pop_back();
                operations++;
                DecorateChunk(*chunk);
            } else if (!queue_.IsEmpty()) {
                do {
                    // Checking if the chunk exists now spreads out the cost of
                    // checking all chunks when the queue is updated over multiple frames
                    glm::ivec2 position = queue_.Pop();
                    if (HasChunk(position)) {
                        continue;
                    }

                    operations++;
                    GenerateChunk(position);
                    break;
                } while (!queue_.IsEmpty());
            } else {
                break;
            }
        }
    }

    Region Manager::GetRegion(const glm::ivec2& position) noexcept {
        return Region({{
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

    void Manager::GenerateChunk(const glm::ivec2& position) noexcept {
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

    void Manager::ShapeChunk(Chunk& chunk) noexcept {
        generator_.Shape(chunk);
        chunk.SetStage(Stage::Shaped);

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

    void Manager::DecorateChunk(Chunk& chunk) noexcept {
        Region region = GetRegion(chunk);
        generator_.Decorate(chunk, region);
        chunk.SetStage(Stage::Decorated);

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

    void Manager::MeshChunk(Chunk& chunk) noexcept {
        Region region = GetRegion(chunk);
        chunk.SetMesh(mesher_.MeshChunk(chunk, region));
        chunk.SetStage(Stage::Meshed);
    }

    bool Manager::IsDecoratable(const Chunk& chunk) const noexcept {
        if (chunk.GetStage() != Stage::Shaped) {
            return false;
        }

        for (const auto& offset : CHUNK_OFFSETS) {
            auto other_chunk = chunks_.find(chunk.GetPosition() + offset);
            if (other_chunk == chunks_.end() || other_chunk->second->GetStage() < Stage::Shaped) {
                return false;
            }
        }

        return true;
    }

    bool Manager::IsMeshable(const Chunk& chunk) const noexcept {
        if (chunk.GetStage() != Stage::Decorated) {
            return false;
        }

        for (const auto& offset : CHUNK_OFFSETS) {
            auto other_chunk = chunks_.find(chunk.GetPosition() + offset);
            if (other_chunk == chunks_.end() || other_chunk->second->GetStage() < Stage::Decorated) {
                return false;
            }
        }

        return true;
    }
    
}
