#ifndef VOXELS_WORLD_CHUNK_MANAGER_H_
#define VOXELS_WORLD_CHUNK_MANAGER_H_

#include "world/chunk.h"
#include "util/ivec2_hash.h"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>

namespace voxels::world {

class ChunkManager {
public:
    ChunkManager() = default;
    ~ChunkManager() = default;

    ChunkManager(const ChunkManager&) = delete;
    ChunkManager& operator=(const ChunkManager&) = delete;
    ChunkManager(ChunkManager&&) = delete;
    ChunkManager& operator=(ChunkManager&&) = delete;

    bool HasChunk(const glm::ivec2& position) const noexcept;
    bool HasNeighbors(const glm::ivec2& position) const noexcept;

    Chunk& GetChunk(const glm::ivec2& position) const;

    void Update(const glm::vec3& player_position) noexcept;

private:
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>, voxels::util::IVec2Hash> chunks_;

    void GenerateChunk(const glm::ivec2& position);
};

}

#endif // VOXELS_WORLD_CHUNK_MANAGER_H_
