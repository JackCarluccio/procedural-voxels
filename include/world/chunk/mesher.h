#pragma once

#include "graphics/mesh.h"
#include "world/chunk/chunk.h"
#include "world/chunk/region.h"
#include "world/block/face.h"

#include <array>

namespace voxels::world::chunk {

    class Mesher {
    public:
        Mesher() = default;

        Mesher(const Mesher&) = delete;
        Mesher& operator=(const Mesher&) = delete;
        Mesher(Mesher&&) = delete;
        Mesher& operator=(Mesher&&) = delete;

        graphics::Mesh MeshChunk(const Chunk& chunk, const Region& region) noexcept;

    private:
        int vertex_count_;
        int index_count_;
        std::array<uint32_t, 0xFFFF * 4 / 6 + 1> vertices_;
        std::array<uint16_t, 0xFFFF> indices_;

        void MeshInterior(const Chunk& chunk) noexcept;
        void MeshExterior(const Chunk& chunk, const Region& region) noexcept;
        void MeshTop(const Chunk& chunk, const Region& region) noexcept;
        void MeshBottom(const Chunk& chunk, const Region& region) noexcept;

        void AddFace(int index, block::Face face, block::Block block) noexcept;
        void AddFace(const Chunk& chunk, int index, block::Face face, block::Block block) noexcept;
        void AddFace(const Region& region, int x, int y, int z, block::Face face, block::Block block) noexcept;

    };
    
}
