#pragma once

#include "graphics/mesh.h"
#include "world/chunk.h"
#include "world/chunk_region.h"
#include "world/face.h"

#include <array>

namespace voxels::world {

    class ChunkMesher {
    public:
        ChunkMesher() = default;

        ChunkMesher(const ChunkMesher&) = delete;
        ChunkMesher& operator=(const ChunkMesher&) = delete;
        ChunkMesher(ChunkMesher&&) = delete;
        ChunkMesher& operator=(ChunkMesher&&) = delete;

        graphics::Mesh MeshChunk(const Chunk& chunk, const ChunkRegion& region) noexcept;

    private:
        int vertex_count_;
        int index_count_;
        std::array<uint32_t, 0xFFFF * 4 / 6 + 1> vertices_;
        std::array<uint16_t, 0xFFFF> indices_;

        void MeshInterior(const Chunk& chunk) noexcept;
        void MeshExterior(const Chunk& chunk, const ChunkRegion& region) noexcept;
        void MeshTop(const Chunk& chunk, const ChunkRegion& region) noexcept;
        void MeshBottom(const Chunk& chunk, const ChunkRegion& region) noexcept;

        void AddFace(int index, Face face, Block block) noexcept;
        void AddFace(const Chunk& chunk, int index, Face face, Block block) noexcept;
        void AddFace(const ChunkRegion& region, int x, int y, int z, Face face, Block block) noexcept;

    };
    
}
