#ifndef VOXELS_WORLD_CHUNK_MANAGER_H_
#define VOXELS_WORLD_CHUNK_MANAGER_H_

#include "world/chunk.h"
#include "world/chunk_generator.h"
#include "world/chunk_mesher.h"
#include "world/chunk_queue.h"
#include "util/ivec2_hash.h"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

namespace voxels::world {

class ChunkManager {
public:
    explicit ChunkManager();
    ~ChunkManager() = default;

    ChunkManager(const ChunkManager&) = delete;
    ChunkManager& operator=(const ChunkManager&) = delete;
    ChunkManager(ChunkManager&&) = delete;
    ChunkManager& operator=(ChunkManager&&) = delete;

    void Init() const noexcept;

    bool HasChunk(const glm::ivec2& position) const noexcept;
    bool HasCardinalNeighbors(const glm::ivec2& position) const noexcept;

    Chunk& GetChunk(const glm::ivec2& position) const;

    void Update(const glm::vec3& player_position) noexcept;

    const std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash>& GetMap() const noexcept {
        return chunks_;
    };

private:
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash> chunks_;
    std::vector<Chunk*> chunks_to_decorate_;
    std::vector<Chunk*> chunks_to_mesh_;

    std::unique_ptr<ChunkGenerator> chunk_generator_;
    std::unique_ptr<ChunkMesher> chunk_mesher_;
    std::unique_ptr<ChunkQueue> chunk_queue_;

    std::array<const Chunk* const, 4> GetCardinalNeighborPointers(const glm::ivec2& position) const;

    void GenerateChunk(const glm::ivec2& position);
    void ShapeChunk(Chunk& chunk) noexcept;
    void DecorateChunk(Chunk& chunk) noexcept;
    void MeshChunk(Chunk& chunk) noexcept;

    bool IsDecoratable(const Chunk& chunk) const noexcept;
    bool IsMeshable(const Chunk& chunk) const noexcept;
};

}

#endif // VOXELS_WORLD_CHUNK_MANAGER_H_
