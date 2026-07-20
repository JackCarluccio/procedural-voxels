#include "world/chunk_mesher.h"

#include "world/block.h"
#include "world/block_data.h"
#include "world/helper.h"

#include "graphics/graphics_core.h"

#include <cstdint>

namespace {

    using namespace voxels::world;

    constexpr uint32_t VertexIndex(int x, int y, int z) noexcept {
        return z + x * 32 + y * 32 * 32;
    }

    constexpr uint32_t VertexIndex(int index) noexcept {
        return (index & 0xF) | ((index & 0xF0) << 1) | ((index & 0xFF00) << 2);
    }

    constexpr uint32_t face_vertex_offsets[6][4] = {
        { VertexIndex(1, 0, 0), VertexIndex(0, 0, 0), VertexIndex(0, 1, 0), VertexIndex(1, 1, 0) }, // -Z
        { VertexIndex(0, 0, 1), VertexIndex(1, 0, 1), VertexIndex(1, 1, 1), VertexIndex(0, 1, 1) }, // +Z
        { VertexIndex(0, 0, 0), VertexIndex(0, 0, 1), VertexIndex(0, 1, 1), VertexIndex(0, 1, 0) }, // -X
        { VertexIndex(1, 0, 1), VertexIndex(1, 0, 0), VertexIndex(1, 1, 0), VertexIndex(1, 1, 1) }, // +X
        { VertexIndex(1, 0, 1), VertexIndex(0, 0, 1), VertexIndex(0, 0, 0), VertexIndex(1, 0, 0) }, // -Y
        { VertexIndex(1, 1, 0), VertexIndex(0, 1, 0), VertexIndex(0, 1, 1), VertexIndex(1, 1, 1) }, // +Y
    };

    constexpr int ambient_occlusion_index_offsets[6][4][3] = {
        // Face::NegZ
        {
            { ToIndex( 1,  0, -1), ToIndex( 1, -1, -1), ToIndex( 0, -1, -1) },
            { ToIndex( 0, -1, -1), ToIndex(-1, -1, -1), ToIndex(-1,  0, -1) },
            { ToIndex(-1,  0, -1), ToIndex(-1,  1, -1), ToIndex( 0,  1, -1) },
            { ToIndex( 0,  1, -1), ToIndex( 1,  1, -1), ToIndex( 1,  0, -1) },
        },
        // Face::PosZ
        {
            { ToIndex(-1,  0,  1), ToIndex(-1, -1,  1), ToIndex( 0, -1,  1) },
            { ToIndex( 0, -1,  1), ToIndex( 1, -1,  1), ToIndex( 1,  0,  1) },
            { ToIndex( 1,  0,  1), ToIndex( 1,  1,  1), ToIndex( 0,  1,  1) },
            { ToIndex( 0,  1,  1), ToIndex(-1,  1,  1), ToIndex(-1,  0,  1) },
        },
        // Face::NegX
        {
            { ToIndex(-1,  0, -1), ToIndex(-1, -1, -1), ToIndex(-1, -1,  0) },
            { ToIndex(-1, -1,  0), ToIndex(-1, -1,  1), ToIndex(-1,  0,  1) },
            { ToIndex(-1,  0,  1), ToIndex(-1,  1,  1), ToIndex(-1,  1,  0) },
            { ToIndex(-1,  1,  0), ToIndex(-1,  1, -1), ToIndex(-1,  0, -1) },
        },
        // Face::PosX
        {
            { ToIndex( 1,  0,  1), ToIndex( 1, -1,  1), ToIndex( 1, -1,  0) },
            { ToIndex( 1, -1,  0), ToIndex( 1, -1, -1), ToIndex( 1,  0, -1) },
            { ToIndex( 1,  0, -1), ToIndex( 1,  1, -1), ToIndex( 1,  1,  0) },
            { ToIndex( 1,  1,  0), ToIndex( 1,  1,  1), ToIndex( 1,  0,  1) },
        },
        // Face::NegY
        {
            { ToIndex( 1, -1,  0), ToIndex( 1, -1,  1), ToIndex( 0, -1,  1) },
            { ToIndex( 0, -1,  1), ToIndex(-1, -1,  1), ToIndex(-1, -1,  0) },
            { ToIndex(-1, -1,  0), ToIndex(-1, -1, -1), ToIndex( 0, -1, -1) },
            { ToIndex( 0, -1, -1), ToIndex( 1, -1, -1), ToIndex( 1, -1,  0) },
        },
        // Face::PosY
        {
            { ToIndex( 1,  1,  0), ToIndex( 1,  1, -1), ToIndex( 0,  1, -1) },
            { ToIndex( 0,  1, -1), ToIndex(-1,  1, -1), ToIndex(-1,  1,  0) },
            { ToIndex(-1,  1,  0), ToIndex(-1,  1,  1), ToIndex( 0,  1,  1) },
            { ToIndex( 0,  1,  1), ToIndex( 1,  1,  1), ToIndex( 1,  1,  0) },
        },
    };

    // Since ambient occlusion is determined from state of 3 blocks, there are 8 possible states.
    // This maps the state to the corresponding ambient occlusion level (0-3).
    constexpr int ambient_occlusion_level[8] = {
        0, // 0b000
        1, // 0b001
        1, // 0b010
        2, // 0b011
        1, // 0b100
        3, // 0b101
        2, // 0b110
        3, // 0b111
    };

    struct ExteriorBlockFaceBlueprint {
        int this_block_index;
        int that_block_index;
    };

    struct ExteriorBlockBlueprint {
        int this_block_index;
        bool has_other_block[6];
    };

    ExteriorBlockFaceBlueprint exterior_block_face_blueprints[4][BLOCKS_PER_CHUNK_WALL] = {
        {}, // -Z
        {}, // +Z
        {}, // -X
        {}, // +X
    };

    ExteriorBlockBlueprint exterior_block_blueprints[BLOCKS_PER_CHUNK_EXTERIOR] = {};

    void BuildExteriorBlockFaceBlueprints() noexcept {
        for (int x = 0; x < CHUNK_WIDTH; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                exterior_block_face_blueprints[0][x + y * CHUNK_WIDTH] = ExteriorBlockFaceBlueprint{
                    .this_block_index = ToIndex(x, y, 0),
                    .that_block_index = ToIndex(x, y, CHUNK_WIDTH - 1)
                };
                exterior_block_face_blueprints[1][x + y * CHUNK_WIDTH] = ExteriorBlockFaceBlueprint{
                    .this_block_index = ToIndex(x, y, CHUNK_WIDTH - 1),
                    .that_block_index = ToIndex(x, y, 0)
                };
            }
        }

        for (int z = 0; z < CHUNK_WIDTH; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                exterior_block_face_blueprints[2][z + y * CHUNK_WIDTH] = ExteriorBlockFaceBlueprint{
                    .this_block_index = ToIndex(0, y, z),
                    .that_block_index = ToIndex(CHUNK_WIDTH - 1, y, z)
                };
                exterior_block_face_blueprints[3][z + y * CHUNK_WIDTH] = ExteriorBlockFaceBlueprint{
                    .this_block_index = ToIndex(CHUNK_WIDTH - 1, y, z),
                    .that_block_index = ToIndex(0, y, z)
                };
            }
        }
    }

    void BuildExteriorBlockBlueprints() noexcept {
        int blueprint_index = 0;
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_WIDTH; z++) {
                for (int x = 0; x < CHUNK_WIDTH; x++) {
                    if (IsInterior(x, y, z)) {
                        continue;
                    }

                    int index = ToIndex(x, y, z);
                    ExteriorBlockBlueprint& blueprint = exterior_block_blueprints[blueprint_index++];
                    blueprint.this_block_index = index;

                    for (int i = 0; i < 6; i++) {
                        blueprint.has_other_block[i] = !IsOutOfBounds(ToCell(index) + ToCell(BLOCK_INDEX_OFFSETS[i]));
                    }
                }
            }
        }
    }

}

namespace voxels::world {

    void ChunkMesher::Init() const noexcept {
        BuildExteriorBlockFaceBlueprints();
        BuildExteriorBlockBlueprints();
    }

    std::unique_ptr<graphics::Mesh> ChunkMesher::MeshChunk(const Chunk& chunk, const std::array<const Chunk*, 4>& neighbors) noexcept {
        vertex_count_ = 0;
        index_count_ = 0;
        
        MeshInterior(chunk);
        MeshTopFaces(chunk);
        MeshExteriorBlockFaces(chunk);
        MeshExteriorFaces(chunk, neighbors[0], Face::NegZ);
        MeshExteriorFaces(chunk, neighbors[1], Face::PosZ);
        MeshExteriorFaces(chunk, neighbors[2], Face::NegX);
        MeshExteriorFaces(chunk, neighbors[3], Face::PosX);

        auto mesh = std::make_unique<graphics::Mesh>(
            vertices_.data(),
            vertex_count_ * sizeof(uint32_t),
            indices_.data(),
            index_count_ * sizeof(uint16_t),
            index_count_
        );

        mesh->LinkAttributeI(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), reinterpret_cast<const void*>(0));

        return mesh;
    }

    void ChunkMesher::MeshInterior(const Chunk& chunk) noexcept {
        for (int y = 1; y < CHUNK_HEIGHT - 1; y++) {
            for (int x = 1; x < CHUNK_WIDTH - 1; x++) {
                for (int z = 1; z < CHUNK_WIDTH - 1; z++) {
                    int index = ToIndex(x, y, z);
                    Block block = chunk.GetBlock(index);
                    if (block == Block::Air) {
                        continue;
                    }

                    bool negZ = chunk.GetBlock(index - STRIDE_Z) == Block::Air;
                    bool posZ = chunk.GetBlock(index + STRIDE_Z) == Block::Air;
                    bool negX = chunk.GetBlock(index - STRIDE_X) == Block::Air;
                    bool posX = chunk.GetBlock(index + STRIDE_X) == Block::Air;
                    bool negY = chunk.GetBlock(index - STRIDE_Y) == Block::Air;
                    bool posY = chunk.GetBlock(index + STRIDE_Y) == Block::Air;

                    // if (negZ) AddFace(index, Face::NegZ, block);
                    if (negZ) Debug_AddFace(chunk, index, Face::NegZ, block);
                    // if (posZ) AddFace(index, Face::PosZ, block);
                    if (posZ) Debug_AddFace(chunk, index, Face::PosZ, block);
                    // if (negX) AddFace(index, Face::NegX, block);
                    if (negX) Debug_AddFace(chunk, index, Face::NegX, block);
                    // if (posX) AddFace(index, Face::PosX, block);
                    if (posX) Debug_AddFace(chunk, index, Face::PosX, block);
                    // if (negY) AddFace(index, Face::NegY, block);
                    if (negY) Debug_AddFace(chunk, index, Face::NegY, block);
                    // if (posY) AddFace(index, Face::PosY, block);
                    if (posY) Debug_AddFace(chunk, index, Face::PosY, block);
                }
            }
        }
    }

    void ChunkMesher::MeshTopFaces(const Chunk& chunk) noexcept {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                Block block = chunk.GetBlock(x, CHUNK_HEIGHT - 1, z);
                if (block == Block::Air) {
                    continue;
                }

                AddFace(ToIndex(x, CHUNK_HEIGHT - 1, z), Face::PosY, block);
            }
        }
    }

    void ChunkMesher::MeshExteriorFaces(const Chunk& thisChunk, const Chunk* thatChunk, Face face) noexcept {
        for (const auto& blueprint : exterior_block_face_blueprints[static_cast<int>(face)]) {
            Block this_block = thisChunk.GetBlock(blueprint.this_block_index);
            if (this_block == Block::Air) {
                continue;
            }

            Block that_block = thatChunk->GetBlock(blueprint.that_block_index);
            if (that_block != Block::Air) {
                continue;
            }
            
            AddFace(blueprint.this_block_index, face, this_block);
        }
    }

    void ChunkMesher::MeshExteriorBlockFaces(const Chunk& chunk) noexcept {
        for (const auto& blueprint : exterior_block_blueprints) {
            Block this_block = chunk.GetBlock(blueprint.this_block_index);
            if (this_block == Block::Air) {
                continue;
            }

            for (int i = 0; i < 6; i++) {
                if (!blueprint.has_other_block[i]) {
                    continue;
                }

                Block that_block = chunk.GetBlock(blueprint.this_block_index + BLOCK_INDEX_OFFSETS[i]);
                if (that_block != Block::Air) {
                    continue;
                }

                AddFace(blueprint.this_block_index, static_cast<Face>(i), this_block);
            }
        }
    }

    void ChunkMesher::AddFace(int index, Face face, Block block) noexcept {
        uint8_t texture_index = voxels::world::GetTextureIndex(block, face);

        for (int i = 0; i < 4; i++) {
            uint32_t vertex_data = texture_index << 19;
            vertex_data |= VertexIndex(index) + face_vertex_offsets[static_cast<int>(face)][i];
            vertices_[vertex_count_ + i] = vertex_data;
        }

        indices_[index_count_ + 0] = static_cast<uint16_t>(vertex_count_ + 0);
        indices_[index_count_ + 1] = static_cast<uint16_t>(vertex_count_ + 1);
        indices_[index_count_ + 2] = static_cast<uint16_t>(vertex_count_ + 2);
        indices_[index_count_ + 3] = static_cast<uint16_t>(vertex_count_ + 0);
        indices_[index_count_ + 4] = static_cast<uint16_t>(vertex_count_ + 2);
        indices_[index_count_ + 5] = static_cast<uint16_t>(vertex_count_ + 3);

        vertex_count_ += 4;
        index_count_ += 6;
    }

    void ChunkMesher::Debug_AddFace(const Chunk& chunk, int index, Face face, Block block) noexcept {
        uint8_t texture_index = voxels::world::GetTextureIndex(block, face);

        for (int i = 0; i < 4; i++) {
            bool ao_block_1 = chunk.GetBlock(index + ambient_occlusion_index_offsets[static_cast<int>(face)][i][0]) != Block::Air;
            bool ao_block_2 = chunk.GetBlock(index + ambient_occlusion_index_offsets[static_cast<int>(face)][i][1]) != Block::Air;
            bool ao_block_3 = chunk.GetBlock(index + ambient_occlusion_index_offsets[static_cast<int>(face)][i][2]) != Block::Air;
            int ao_index = ao_block_3 << 2 | ao_block_2 << 1 | ao_block_1;

            uint32_t vertex_data = texture_index << 19;
            vertex_data |= ambient_occlusion_level[ao_index] << 27;
            vertex_data |= VertexIndex(index) + face_vertex_offsets[static_cast<int>(face)][i];
            vertices_[vertex_count_ + i] = vertex_data;
        }

        indices_[index_count_ + 0] = static_cast<uint16_t>(vertex_count_ + 0);
        indices_[index_count_ + 1] = static_cast<uint16_t>(vertex_count_ + 1);
        indices_[index_count_ + 2] = static_cast<uint16_t>(vertex_count_ + 2);
        indices_[index_count_ + 3] = static_cast<uint16_t>(vertex_count_ + 0);
        indices_[index_count_ + 4] = static_cast<uint16_t>(vertex_count_ + 2);
        indices_[index_count_ + 5] = static_cast<uint16_t>(vertex_count_ + 3);

        vertex_count_ += 4;
        index_count_ += 6;
    }

}
