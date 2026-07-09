#pragma once

#include "world/chunk.h"
#include "world/chunk_generator.h"
#include "world/chunk_mesher.h"
#include "world/chunk_queue.h"
#include "world/chunk_region.h"
#include "util/ivec2_hash.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

namespace voxels::world {

    class ChunkManager {
    public:
        ChunkManager();

        ChunkManager(const ChunkManager&) = delete;
        ChunkManager& operator=(const ChunkManager&) = delete;
        ChunkManager(ChunkManager&&) = delete;
        ChunkManager& operator=(ChunkManager&&) = delete;

        void Init() const noexcept;
        void Update(const glm::vec3& player_position) noexcept;

        bool HasChunk(const glm::ivec2& position) const noexcept {
            return chunks_.find(position) != chunks_.end();
        };

        Chunk& GetChunk(const glm::ivec2& position) noexcept {
            return *chunks_.find(position)->second;
        };

        const Chunk& GetChunk(const glm::ivec2& position) const noexcept {
            return *chunks_.find(position)->second;
        };

        ChunkRegion GetChunkRegion(const glm::ivec2& position) noexcept;
        ChunkRegion GetChunkRegion(const Chunk& chunk) noexcept;

        const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash>& GetMap() noexcept {
            return chunks_;
        };

    private:
        std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash> chunks_;
        std::vector<Chunk*> chunks_to_decorate_;
        std::vector<Chunk*> chunks_to_mesh_;

        std::unique_ptr<ChunkGenerator> chunk_generator_;
        std::unique_ptr<ChunkMesher> chunk_mesher_;
        std::unique_ptr<ChunkQueue> chunk_queue_;

        std::array<const Chunk*, 4> GetCardinalNeighborPointers(const glm::ivec2& position) const noexcept;

        void GenerateChunk(const glm::ivec2& position) noexcept;
        void ShapeChunk(Chunk& chunk) noexcept;
        void DecorateChunk(Chunk& chunk) noexcept;
        void MeshChunk(Chunk& chunk) noexcept;

        bool IsDecoratable(const Chunk& chunk) const noexcept;
        bool IsMeshable(const Chunk& chunk) const noexcept;
    };

}
