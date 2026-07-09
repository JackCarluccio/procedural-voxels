#pragma once

#include "graphics/mesh.h"
#include "world/chunk.h"
#include "world/face.h"

#include <array>
#include <memory>

namespace voxels::world {

    class ChunkMesher {
    public:
        ChunkMesher() = default;

        ChunkMesher(const ChunkMesher&) = delete;
        ChunkMesher& operator=(const ChunkMesher&) = delete;
        ChunkMesher(ChunkMesher&&) = delete;
        ChunkMesher& operator=(ChunkMesher&&) = delete;

        void Init() const noexcept;

        std::unique_ptr<graphics::Mesh> MeshChunk(const Chunk& chunk, const std::array<const Chunk*, 4>& neighbors) noexcept;

    private:
        int vertex_count_;
        int index_count_;
        std::array<uint32_t, BLOCKS_PER_CHUNK / 2 * 6 * 4> vertices_;
        std::array<uint16_t, BLOCKS_PER_CHUNK / 2 * 6 * 6> indices_;

        void MeshInterior(const Chunk& chunk) noexcept;
        void MeshTopFaces(const Chunk& chunk) noexcept;
        void MeshExteriorBlockFaces(const Chunk& chunk) noexcept;
        void MeshExteriorFaces(const Chunk& chunk, const Chunk* neighbor, Face face) noexcept;

        void AddFace(int index, Face face, Block block) noexcept;

    };
    
}
