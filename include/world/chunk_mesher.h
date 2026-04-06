#ifndef VOXELS_WORLD_CHUNK_MESHER_H_
#define VOXELS_WORLD_CHUNK_MESHER_H_

#include "graphics/mesh.h"
#include "world/chunk.h"

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

    std::unique_ptr<graphics::Mesh> MeshChunk(const Chunk& chunk);
};
    
}

#endif // VOXELS_WORLD_CHUNK_MESHER_H_
