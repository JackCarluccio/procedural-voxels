#include "world/chunk/manager.h"

#include "graphics/camera.h"
#include "world/chunk/stage.h"

#include <algorithm>
#include <cmath>
#include <tuple>
#include <utility>

#include <glm/vec2.hpp>

namespace {

    constexpr int RENDER_RADIUS = 16;
    constexpr int OPERATIONS_PER_UPDATE = 8;

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

    Manager::Manager(const graphics::Camera* camera) noexcept
        : generator_(),
        mesher_(),
        queue_(RENDER_RADIUS, camera)
    {}

    void Manager::Init() noexcept {
        generator_.Init();
    }

    void Manager::Update(const glm::vec3& player_position) noexcept {
        const glm::ivec2 player_chunk_position = glm::ivec2(
            static_cast<int>(std::floor(player_position.x / Chunk::WIDTH)),
            static_cast<int>(std::floor(player_position.z / Chunk::WIDTH))
        );

        queue_.Update(player_chunk_position);
        DeleteChunks(player_chunk_position);
        CreateChunks();

        last_chunk_position_ = player_chunk_position;
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

    void Manager::CreateChunks() noexcept {
        int operations = 0;
        while (operations < OPERATIONS_PER_UPDATE) {
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

    void Manager::DeleteChunks(const glm::ivec2& player_chunk_position) noexcept {
        if (player_chunk_position == last_chunk_position_) {
            return;
        }

        // Traveling in axis-unaligned directions will alternate player chunk positions perpendicular to direction
        // Causing excess deletion. Add a slight buffer to minimize this effect.
        int threshold = (RENDER_RADIUS + 1) * (RENDER_RADIUS + 1);

        for (auto it = chunks_.begin(); it != chunks_.end();) {
            auto& position = it->first;
            int dx = position.x - player_chunk_position.x;
            int dz = position.y - player_chunk_position.y;
            if (dx * dx + dz * dz > threshold) {
                it = DeleteChunk(it);
            } else {
                ++it;
            }
        }
    }

    Manager::ChunkMap::iterator Manager::DeleteChunk(ChunkMap::iterator it) noexcept {
        Chunk& chunk = *it->second;
        RemoveChunkFromDecorateQueue(&chunk);
        RemoveChunkFromMeshQueue(&chunk);
        DowngradeNeighbors(chunk, Stage::Shaped);
        return chunks_.erase(it);
    }

    void Manager::RemoveChunkFromDecorateQueue(Chunk* chunk) noexcept {
        auto it = std::ranges::find(chunks_to_decorate_, chunk);
        if (it != chunks_to_decorate_.end()) {
            chunks_to_decorate_.erase(it);
        }
    }

    void Manager::RemoveChunkFromMeshQueue(Chunk* chunk) noexcept {
        auto it = std::ranges::find(chunks_to_mesh_, chunk);
        if (it != chunks_to_mesh_.end()) {
            chunks_to_mesh_.erase(it);
        }
    }

    void Manager::DowngradeNeighbors(Chunk& chunk, Stage max_stage) noexcept {
        for (const auto& offset : CHUNK_OFFSETS) {
            auto it = chunks_.find(chunk.GetPosition() + offset);
            if (it == chunks_.end()) {
                continue;
            }

            bool downgraded = Downgrade(*it->second, max_stage);
            if (downgraded) {
                DowngradeNeighbors(*it->second, NextStage(max_stage));
            }
        }
    }

    bool Manager::Downgrade(Chunk& chunk, Stage max_stage) noexcept {
        Stage stage = chunk.GetStage();

        // If Downgrade() is called on a chunk, its because one of its 8 neighbors have been downgraded.
        // That means this chunk no longer meets the requirements to advance to its next stage.
        // So, remove it from any processing queues regardless if we're actually downgrading this chunk
        if (stage == Stage::Decorated) {
            RemoveChunkFromMeshQueue(&chunk);
        } else if (stage == Stage::Shaped) {
            RemoveChunkFromDecorateQueue(&chunk);
        }

        if (stage <= max_stage) {
            return false;
        }

        chunk.DeleteMesh(); // Safe to call regardless if the mesh exists

        if (max_stage < Stage::Decorated) {
            // Remove decorations by re-shaping the chunk
            generator_.Shape(chunk);
        }

        chunk.SetStage(max_stage);
        return true;
    }

}
