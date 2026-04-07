#include "world/chunk_mesher.h"

#include "world/block.h"
#include "world/block_data.h"

#include "graphics/graphics_core.h"

#include <cstdint>
#include <vector>

namespace {

struct VertexData {
    float x, y, z;
    float u, v;
    int texture_index;
};

constexpr float face_vertex_offsets[6][4][3] = {
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, // -Z
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, // +Z
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}, // -X
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}, // +X
    {{1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 0, 0}}, // -Y
    {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}, // +Y
};

constexpr float vertex_uvs[4][2] = {
    {0.0f / 16.0f, 0.0f / 16.0f},
    {1.0f / 16.0f, 0.0f / 16.0f},
    {1.0f / 16.0f, 1.0f / 16.0f},
    {0.0f / 16.0f, 1.0f / 16.0f}
};

void AddFace(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, int x, int y, int z, int face, voxels::world::Block block) noexcept {
    int vertex_index_start = static_cast<int>(vertices.size());
    int texture_index = voxels::world::GetTextureIndex(block, face);

    for (int i = 0; i < 4; i++) {
        vertices.emplace_back(VertexData{
            x + face_vertex_offsets[face][i][0],
            y + face_vertex_offsets[face][i][1],
            z + face_vertex_offsets[face][i][2],
            vertex_uvs[i][0],
            vertex_uvs[i][1],
            texture_index
        });
    }

    indices.push_back(vertex_index_start + 0);
    indices.push_back(vertex_index_start + 1);
    indices.push_back(vertex_index_start + 2);
    indices.push_back(vertex_index_start + 0);
    indices.push_back(vertex_index_start + 2);
    indices.push_back(vertex_index_start + 3);
}

void MeshInterior(const voxels::world::Chunk& chunk, std::vector<VertexData>& vertices, std::vector<uint32_t>& indices) noexcept {
    for (int y = 0; y < voxels::world::Chunk::HEIGHT; y++) {
        for (int x = 0; x < voxels::world::Chunk::SIZE; x++) {
            for (int z = 0; z < voxels::world::Chunk::SIZE; z++) {
                voxels::world::Block block = chunk.GetBlock(x, y, z);
                if (block == voxels::world::Block::Air) {
                    continue;
                }

                for (int face = 0; face < 6; face++) {
                    AddFace(vertices, indices, x, y, z, face, block);
                }
            }
        }
    }
}

} // namespace

namespace voxels::world {

std::unique_ptr<graphics::Mesh> ChunkMesher::MeshChunk(const Chunk& chunk) {
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;

    MeshInterior(chunk, vertices, indices);

    std::unique_ptr<graphics::Mesh> mesh = std::make_unique<graphics::Mesh>(
        vertices.data(),
        vertices.size() * sizeof(VertexData),
        indices.data(),
        indices.size() * sizeof(uint32_t),
        static_cast<int>(indices.size())
    );

    mesh->LinkAttribute(0, 3, GL_FLOAT, sizeof(VertexData), reinterpret_cast<void*>(offsetof(VertexData, x)));
    mesh->LinkAttribute(1, 2, GL_FLOAT, sizeof(VertexData), reinterpret_cast<void*>(offsetof(VertexData, u)));
    mesh->LinkAttributeI(2, 1, GL_INT, sizeof(VertexData), reinterpret_cast<void*>(offsetof(VertexData, texture_index)));

    return mesh;
}

} // namespace voxels::world
