#include "world/chunk_mesher.h"

#include "world/block.h"
#include "world/block_data.h"
#include "world/face.h"
#include "world/helper.h"

#include "graphics/graphics_core.h"

#include <cstdint>
#include <vector>

namespace {

using namespace voxels::world;

struct VertexData {
    float x, y, z;
    float u, v;
    int texture_index;
};

struct ExteriorBlockFaceBlueprint {
    int this_block_index;
    int that_block_index;
};

struct ExteriorBlockBlueprint {
    int this_block_index;
    int num_faces;
    struct {
        int that_block_index;
        Face face;
    } faces[5];
};

ExteriorBlockFaceBlueprint exterior_block_face_blueprints[4][BLOCKS_PER_CHUNK_WALL] = {
    {}, // -Z
    {}, // +Z
    {}, // -X
    {}, // +X
};

ExteriorBlockBlueprint exterior_block_blueprints[BLOCKS_PER_CHUNK_EXTERIOR]= {};

constexpr float face_vertex_offsets[6][4][3] = {
    {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}, {1, 1, 0}}, // -Z
    {{0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}}, // +Z
    {{0, 0, 0}, {0, 0, 1}, {0, 1, 1}, {0, 1, 0}}, // -X
    {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}, // +X
    {{1, 0, 0}, {1, 0, 1}, {0, 0, 1}, {0, 0, 0}}, // -Y
    {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}, {1, 1, 0}}, // +Y
};

constexpr float vertex_uvs[4][2] = {
    {0.0f / 16.0f, 1.0f / 16.0f},
    {1.0f / 16.0f, 1.0f / 16.0f},
    {1.0f / 16.0f, 0.0f / 16.0f},
    {0.0f / 16.0f, 0.0f / 16.0f}
};

void AddFace(std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, const glm::ivec3& cell, Face face, Block block) noexcept {
    int vertex_index_start = static_cast<int>(vertices.size());
    int texture_index = voxels::world::GetTextureIndex(block, face);

    for (int i = 0; i < 4; i++) {
        vertices.emplace_back(VertexData{
            cell.x + face_vertex_offsets[static_cast<int>(face)][i][0],
            cell.y + face_vertex_offsets[static_cast<int>(face)][i][1],
            cell.z + face_vertex_offsets[static_cast<int>(face)][i][2],
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

void MeshInterior(const Chunk& chunk, std::vector<VertexData>& vertices, std::vector<uint32_t>& indices) noexcept {
    for (int y = 1; y < CHUNK_HEIGHT - 1; y++) {
        for (int x = 1; x < CHUNK_SIZE - 1; x++) {
            for (int z = 1; z < CHUNK_SIZE - 1; z++) {
                int index = ToIndex(x, y, z);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                for (int face = 0; face < 6; face++) {
                    int neighbor_index = index + CARDINAL_INDEX_OFFSETS[face];
                    Block neighbor_block = chunk.GetBlock(neighbor_index);
                    if (neighbor_block != Block::Air) {
                        continue;
                    }

                    AddFace(vertices, indices, ToCell(index), static_cast<Face>(face), block);
                }
            }
        }
    }
}

void MeshExteriorFaces(const Chunk& thisChunk, const Chunk* const thatChunk, std::vector<VertexData>& vertices, std::vector<uint32_t>& indices, Face face) noexcept {
    for (const auto& blueprint : exterior_block_face_blueprints[static_cast<int>(face)]) {
        Block this_block = thisChunk.GetBlock(blueprint.this_block_index);
        if (this_block == Block::Air) {
            continue;
        }

        Block that_block = thatChunk->GetBlock(blueprint.that_block_index);
        if (that_block != Block::Air) {
            continue;
        }

        AddFace(vertices, indices, ToCell(blueprint.this_block_index), face, this_block);
    }
}

void MeshExteriorBlockFaces(const Chunk& chunk, std::vector<VertexData>& vertices, std::vector<uint32_t>& indices) noexcept {
    for (const auto& blueprint : exterior_block_blueprints) {
        Block this_block = chunk.GetBlock(blueprint.this_block_index);
        if (this_block == Block::Air) {
            continue;
        }

        for (int i = 0; i < blueprint.num_faces; i++) {
            const auto& face_info = blueprint.faces[i];
            Block that_block = chunk.GetBlock(face_info.that_block_index);
            if (that_block != Block::Air) {
                continue;
            }

            AddFace(vertices, indices, ToCell(blueprint.this_block_index), face_info.face, this_block);
        }
    }
}

void MeshTopFaces(const Chunk& chunk, std::vector<VertexData>& vertices, std::vector<uint32_t>& indices) noexcept {
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            Block block = chunk.GetBlock(x, CHUNK_HEIGHT - 1, z);
            if (block == Block::Air) {
                continue;
            }

            AddFace(vertices, indices, ToCell(x, CHUNK_HEIGHT - 1, z), Face::PosY, block);
        }
    }
}

void BuildExteriorBlockFaceBlueprints() noexcept {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            exterior_block_face_blueprints[0][x + y * CHUNK_SIZE] = ExteriorBlockFaceBlueprint{
                .this_block_index = ToIndex(x, y, 0),
                .that_block_index = ToIndex(x, y, CHUNK_SIZE - 1)
            };
            exterior_block_face_blueprints[1][x + y * CHUNK_SIZE] = ExteriorBlockFaceBlueprint{
                .this_block_index = ToIndex(x, y, CHUNK_SIZE - 1),
                .that_block_index = ToIndex(x, y, 0)
            };
        }
    }

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            exterior_block_face_blueprints[2][z + y * CHUNK_SIZE] = ExteriorBlockFaceBlueprint{
                .this_block_index = ToIndex(0, y, z),
                .that_block_index = ToIndex(CHUNK_SIZE - 1, y, z)
            };
            exterior_block_face_blueprints[3][z + y * CHUNK_SIZE] = ExteriorBlockFaceBlueprint{
                .this_block_index = ToIndex(CHUNK_SIZE - 1, y, z),
                .that_block_index = ToIndex(0, y, z)
            };
        }
    }
}

void BuildExteriorBlockBlueprints() noexcept {
    int blueprint_index = 0;
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                if (IsInterior(x, y, z)) {
                    continue;
                }

                int index = ToIndex(x, y, z);
                ExteriorBlockBlueprint& blueprint = exterior_block_blueprints[blueprint_index++];
                blueprint = ExteriorBlockBlueprint{
                    .this_block_index = index,
                    .num_faces = 0,
                    .faces = {}
                };

                const auto cell = ToCell(index);
                for (int i = 0; i < 6; i++) {
                    const auto& offset = CARDINAL_VECTOR_OFFSETS[i];
                    if (IsOutOfBounds(cell + offset)) {
                        continue;
                    }

                    blueprint.num_faces++;
                    blueprint.faces[blueprint.num_faces - 1] = {
                        .that_block_index = index + ToIndex(offset),
                        .face = static_cast<Face>(i),
                    };
                }
            }
        }
    }
}

} // namespace

namespace voxels::world {

void ChunkMesher::Init() const noexcept {
    BuildExteriorBlockFaceBlueprints();
    BuildExteriorBlockBlueprints();
}

std::unique_ptr<graphics::Mesh> ChunkMesher::MeshChunk(const Chunk& chunk, const std::array<const Chunk* const, 4>& neighbors) {
    std::vector<VertexData> vertices;
    std::vector<uint32_t> indices;

    MeshInterior(chunk, vertices, indices);
    MeshExteriorBlockFaces(chunk, vertices, indices);
    MeshTopFaces(chunk, vertices, indices);
    MeshExteriorFaces(chunk, neighbors[0], vertices, indices, Face::NegZ);
    MeshExteriorFaces(chunk, neighbors[1], vertices, indices, Face::PosZ);
    MeshExteriorFaces(chunk, neighbors[2], vertices, indices, Face::NegX);
    MeshExteriorFaces(chunk, neighbors[3], vertices, indices, Face::PosX);

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
