#ifndef VOXELS_WORLD_CHUNK_MESHER_H_
#define VOXELS_WORLD_CHUNK_MESHER_H_

#include "graphics/mesh.h"
#include "world/chunk.h"

#include <array>
#include <memory>

namespace voxels::world {

class ChunkMesher {
public:
    ChunkMesher() = default;
    ~ChunkMesher() = default;

    ChunkMesher(const ChunkMesher&) = delete;
    ChunkMesher& operator=(const ChunkMesher&) = delete;
    ChunkMesher(ChunkMesher&&) = delete;
    ChunkMesher& operator=(ChunkMesher&&) = delete;

    void Init() const noexcept;

    std::unique_ptr<graphics::Mesh> MeshChunk(const Chunk& chunk, const std::array<const Chunk* const, 4>& neighbors);
};
    
}

#endif // VOXELS_WORLD_CHUNK_MESHER_H_
