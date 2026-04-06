#include "world/chunk_mesher.h"

#include "world/block.h"

#include "graphics/graphics_core.h"

#include <cstdint>
#include <vector>

namespace {

float faceVertexOffsets[6][4][3] = {
    // +X
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
    // -X
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
    // +Y
    {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}},
    // -Y
    {{1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 0, 0}},
    // +Z
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
    // -Z
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}
};

void AddFace(std::vector<float>& vertices, std::vector<uint32_t>& indices, int x, int y, int z, int face) noexcept {
    int vertex_index_start = static_cast<int>(vertices.size()) / 3;

    for (int i = 0; i < 4; i++) {
        vertices.push_back(x + faceVertexOffsets[face][i][0]);
        vertices.push_back(y + faceVertexOffsets[face][i][1]);
        vertices.push_back(z + faceVertexOffsets[face][i][2]);
    }

    indices.push_back(vertex_index_start + 0);
    indices.push_back(vertex_index_start + 1);
    indices.push_back(vertex_index_start + 2);
    indices.push_back(vertex_index_start + 0);
    indices.push_back(vertex_index_start + 2);
    indices.push_back(vertex_index_start + 3);
}

void MeshInterior(const voxels::world::Chunk& chunk, std::vector<float>& vertices, std::vector<uint32_t>& indices) noexcept {
    for (int y = 0; y < voxels::world::Chunk::HEIGHT; y++) {
        for (int x = 0; x < voxels::world::Chunk::SIZE; x++) {
            for (int z = 0; z < voxels::world::Chunk::SIZE; z++) {
                if (chunk.GetBlock(x, y, z) == voxels::world::Block::Air) {
                    continue;
                }

                for (int face = 0; face < 6; face++) {
                    AddFace(vertices, indices, x, y, z, face);
                }
            }
        }
    }
}

} // namespace

namespace voxels::world {

std::unique_ptr<graphics::Mesh> ChunkMesher::MeshChunk(const Chunk& chunk) {
    std::vector<float> vertices;
    std::vector<uint32_t> indices;

    MeshInterior(chunk, vertices, indices);

    std::unique_ptr<graphics::Mesh> mesh = std::make_unique<graphics::Mesh>(
        vertices.data(),
        vertices.size() * sizeof(float),
        indices.data(),
        indices.size() * sizeof(uint32_t),
        static_cast<int>(indices.size())
    );
    mesh->LinkAttribute(0, 3, GL_FLOAT, 3 * sizeof(float), nullptr);

    return mesh;
}

} // namespace voxels::world
