#pragma once

#include "graphics/camera.h"
#include "util/ivec2_hash.h"
#include "world/chunk/chunk.h"
#include "world/chunk/generator.h"
#include "world/chunk/mesher.h"
#include "world/chunk/queue.h"
#include "world/chunk/region.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

namespace voxels::graphics {
    class Camera;
}

namespace voxels::world::chunk {

    class Manager {
    public:
        typedef std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash> ChunkMap;

        explicit Manager(const graphics::Camera* camera) noexcept;

        Manager(Manager&&) = delete;
        Manager& operator=(Manager&&) = delete;

        void Init() noexcept;
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

        Region GetRegion(const glm::ivec2& position) noexcept;
        Region GetRegion(const Chunk& chunk) noexcept { return GetRegion(chunk.GetPosition()); }

        const ChunkMap& GetMap() noexcept { return chunks_; };

    private:
        ChunkMap chunks_;
        std::vector<Chunk*> chunks_to_decorate_;
        std::vector<Chunk*> chunks_to_mesh_;

        Generator generator_;
        Mesher mesher_;
        Queue queue_;

        void CreateChunks() noexcept;
        void GenerateChunk(const glm::ivec2& position) noexcept;
        void ShapeChunk(Chunk& chunk) noexcept;
        void DecorateChunk(Chunk& chunk) noexcept;
        void MeshChunk(Chunk& chunk) noexcept;

        bool IsDecoratable(const Chunk& chunk) const noexcept;
        bool IsMeshable(const Chunk& chunk) const noexcept;
        
        void DeleteChunks(const glm::ivec2& player_chunk_position) noexcept;
        ChunkMap::iterator DeleteChunk(ChunkMap::iterator it) noexcept;
        void RemoveChunkFromDecorateQueue(Chunk* chunk) noexcept;
        void RemoveChunkFromMeshQueue(Chunk* chunk) noexcept;
        void DowngradeNeighbors(Chunk& chunk, Stage max_stage) noexcept;
        bool Downgrade(Chunk& chunk, Stage max_stage) noexcept;
    };

}
