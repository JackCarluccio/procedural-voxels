#include "world/chunk/mesher.h"

#include "graphics/graphics_core.h"
#include "world/block.h"
#include "world/block_data.h"

#include <cstdint>

namespace {

    using namespace voxels::world;

    constexpr uint32_t VertexIndex(int x, int y, int z) noexcept {
        return z + x * 32 + y * 32 * 32;
    }

    constexpr uint32_t VertexIndex(int index) noexcept {
        return (index & 0xF) | ((index & 0xF0) << 1) | ((index & 0xFF00) << 2);
    }

    constexpr uint32_t FACE_VERTEX_OFFSETS[6][4] = {
        { VertexIndex(1, 0, 0), VertexIndex(0, 0, 0), VertexIndex(0, 1, 0), VertexIndex(1, 1, 0) }, // -Z
        { VertexIndex(0, 0, 1), VertexIndex(1, 0, 1), VertexIndex(1, 1, 1), VertexIndex(0, 1, 1) }, // +Z
        { VertexIndex(0, 0, 0), VertexIndex(0, 0, 1), VertexIndex(0, 1, 1), VertexIndex(0, 1, 0) }, // -X
        { VertexIndex(1, 0, 1), VertexIndex(1, 0, 0), VertexIndex(1, 1, 0), VertexIndex(1, 1, 1) }, // +X
        { VertexIndex(1, 0, 1), VertexIndex(0, 0, 1), VertexIndex(0, 0, 0), VertexIndex(1, 0, 0) }, // -Y
        { VertexIndex(1, 1, 0), VertexIndex(0, 1, 0), VertexIndex(0, 1, 1), VertexIndex(1, 1, 1) }, // +Y
    };

    struct Point {
        int x, y, z;
    };

    constexpr int XYZToIndex(const Point& xyz) noexcept {
        return chunk::Chunk::ToIndex(xyz.x, xyz.y, xyz.z);
    }

    constexpr Point AO_OFFSETS[6][4][3] = {
        { // Face::NegZ
            { {  1,  0, -1 }, {  1, -1, -1 }, {  0, -1, -1 } },
            { {  0, -1, -1 }, { -1, -1, -1 }, { -1,  0, -1 } },
            { { -1,  0, -1 }, { -1,  1, -1 }, {  0,  1, -1 } },
            { {  0,  1, -1 }, {  1,  1, -1 }, {  1,  0, -1 } },
        },
        { // Face::PosZ
            { { -1,  0,  1 }, { -1, -1,  1 }, {  0, -1,  1 } },
            { {  0, -1,  1 }, {  1, -1,  1 }, {  1,  0,  1 } },
            { {  1,  0,  1 }, {  1,  1,  1 }, {  0,  1,  1 } },
            { {  0,  1,  1 }, { -1,  1,  1 }, { -1,  0,  1 } },
        },
        { // Face::NegX
            { { -1,  0, -1 }, { -1, -1, -1 }, { -1, -1,  0 } },
            { { -1, -1,  0 }, { -1, -1,  1 }, { -1,  0,  1 } },
            { { -1,  0,  1 }, { -1,  1,  1 }, { -1,  1,  0 } },
            { { -1,  1,  0 }, { -1,  1, -1 }, { -1,  0, -1 } },
        },
        { // Face::PosX
            { {  1,  0,  1 }, {  1, -1,  1 }, {  1, -1,  0 } },
            { {  1, -1,  0 }, {  1, -1, -1 }, {  1,  0, -1 } },
            { {  1,  0, -1 }, {  1,  1, -1 }, {  1,  1,  0 } },
            { {  1,  1,  0 }, {  1,  1,  1 }, {  1,  0,  1 } },
        },
        { // Face::NegY
            { {  1, -1,  0 }, {  1, -1,  1 }, {  0, -1,  1 } },
            { {  0, -1,  1 }, { -1, -1,  1 }, { -1, -1,  0 } },
            { { -1, -1,  0 }, { -1, -1, -1 }, {  0, -1, -1 } },
            { {  0, -1, -1 }, {  1, -1, -1 }, {  1, -1,  0 } },
        },
        { // Face::PosY
            { {  1,  1,  0 }, {  1,  1, -1 }, {  0,  1, -1 } },
            { {  0,  1, -1 }, { -1,  1, -1 }, { -1,  1,  0 } },
            { { -1,  1,  0 }, { -1,  1,  1 }, {  0,  1,  1 } },
            { {  0,  1,  1 }, {  1,  1,  1 }, {  1,  1,  0 } },
        },
    };

    constexpr int AO_INDICES[6][4][3] = {
        { // Face::NegZ
            { XYZToIndex(AO_OFFSETS[0][0][0]), XYZToIndex(AO_OFFSETS[0][0][1]), XYZToIndex(AO_OFFSETS[0][0][2]) },
            { XYZToIndex(AO_OFFSETS[0][1][0]), XYZToIndex(AO_OFFSETS[0][1][1]), XYZToIndex(AO_OFFSETS[0][1][2]) },
            { XYZToIndex(AO_OFFSETS[0][2][0]), XYZToIndex(AO_OFFSETS[0][2][1]), XYZToIndex(AO_OFFSETS[0][2][2]) },
            { XYZToIndex(AO_OFFSETS[0][3][0]), XYZToIndex(AO_OFFSETS[0][3][1]), XYZToIndex(AO_OFFSETS[0][3][2]) },
        },
        { // Face::PosZ
            { XYZToIndex(AO_OFFSETS[1][0][0]), XYZToIndex(AO_OFFSETS[1][0][1]), XYZToIndex(AO_OFFSETS[1][0][2]) },
            { XYZToIndex(AO_OFFSETS[1][1][0]), XYZToIndex(AO_OFFSETS[1][1][1]), XYZToIndex(AO_OFFSETS[1][1][2]) },
            { XYZToIndex(AO_OFFSETS[1][2][0]), XYZToIndex(AO_OFFSETS[1][2][1]), XYZToIndex(AO_OFFSETS[1][2][2]) },
            { XYZToIndex(AO_OFFSETS[1][3][0]), XYZToIndex(AO_OFFSETS[1][3][1]), XYZToIndex(AO_OFFSETS[1][3][2]) },
        },
        { // Face::NegX
            { XYZToIndex(AO_OFFSETS[2][0][0]), XYZToIndex(AO_OFFSETS[2][0][1]), XYZToIndex(AO_OFFSETS[2][0][2]) },
            { XYZToIndex(AO_OFFSETS[2][1][0]), XYZToIndex(AO_OFFSETS[2][1][1]), XYZToIndex(AO_OFFSETS[2][1][2]) },
            { XYZToIndex(AO_OFFSETS[2][2][0]), XYZToIndex(AO_OFFSETS[2][2][1]), XYZToIndex(AO_OFFSETS[2][2][2]) },
            { XYZToIndex(AO_OFFSETS[2][3][0]), XYZToIndex(AO_OFFSETS[2][3][1]), XYZToIndex(AO_OFFSETS[2][3][2]) },
        },
        { // Face::PosX
            { XYZToIndex(AO_OFFSETS[3][0][0]), XYZToIndex(AO_OFFSETS[3][0][1]), XYZToIndex(AO_OFFSETS[3][0][2]) },
            { XYZToIndex(AO_OFFSETS[3][1][0]), XYZToIndex(AO_OFFSETS[3][1][1]), XYZToIndex(AO_OFFSETS[3][1][2]) },
            { XYZToIndex(AO_OFFSETS[3][2][0]), XYZToIndex(AO_OFFSETS[3][2][1]), XYZToIndex(AO_OFFSETS[3][2][2]) },
            { XYZToIndex(AO_OFFSETS[3][3][0]), XYZToIndex(AO_OFFSETS[3][3][1]), XYZToIndex(AO_OFFSETS[3][3][2]) },
        },
        { // Face::NegY
            { XYZToIndex(AO_OFFSETS[4][0][0]), XYZToIndex(AO_OFFSETS[4][0][1]), XYZToIndex(AO_OFFSETS[4][0][2]) },
            { XYZToIndex(AO_OFFSETS[4][1][0]), XYZToIndex(AO_OFFSETS[4][1][1]), XYZToIndex(AO_OFFSETS[4][1][2]) },
            { XYZToIndex(AO_OFFSETS[4][2][0]), XYZToIndex(AO_OFFSETS[4][2][1]), XYZToIndex(AO_OFFSETS[4][2][2]) },
            { XYZToIndex(AO_OFFSETS[4][3][0]), XYZToIndex(AO_OFFSETS[4][3][1]), XYZToIndex(AO_OFFSETS[4][3][2]) },
        },
        { // Face::PosY
            { XYZToIndex(AO_OFFSETS[5][0][0]), XYZToIndex(AO_OFFSETS[5][0][1]), XYZToIndex(AO_OFFSETS[5][0][2]) },
            { XYZToIndex(AO_OFFSETS[5][1][0]), XYZToIndex(AO_OFFSETS[5][1][1]), XYZToIndex(AO_OFFSETS[5][1][2]) },
            { XYZToIndex(AO_OFFSETS[5][2][0]), XYZToIndex(AO_OFFSETS[5][2][1]), XYZToIndex(AO_OFFSETS[5][2][2]) },
            { XYZToIndex(AO_OFFSETS[5][3][0]), XYZToIndex(AO_OFFSETS[5][3][1]), XYZToIndex(AO_OFFSETS[5][3][2]) },
        },
    };

    // Maps 3-bit AO state to ambient occlusion level (0-3)
    constexpr int AO_LEVELS[8] = {
        0, // 0b000
        1, // 0b001
        1, // 0b010
        2, // 0b011
        1, // 0b100
        3, // 0b101
        2, // 0b110
        3, // 0b111
    };

}

namespace voxels::world::chunk {

    graphics::Mesh Mesher::MeshChunk(const Chunk& chunk, const Region& region) noexcept {
        vertex_count_ = 0;
        index_count_ = 0;
        
        MeshInterior(chunk);
        MeshExterior(chunk, region);
        MeshTop(chunk, region);
        MeshBottom(chunk, region);

        auto mesh = graphics::Mesh(
            vertices_.data(), vertex_count_,
            indices_.data(), index_count_
        );

        mesh.LinkAttributeI(0, 1, GL_UNSIGNED_INT, sizeof(uint32_t), reinterpret_cast<const void*>(0));

        return mesh;
    }

    void Mesher::MeshInterior(const Chunk& chunk) noexcept {
        for (int y = 1; y < Chunk::HEIGHT - 1; y++) {
            for (int x = 1; x < Chunk::WIDTH - 1; x++) {
                for (int z = 1; z < Chunk::WIDTH - 1; z++) {
                    int index = Chunk::ToIndex(x, y, z);
                    Block block = chunk.GetBlock(index);
                    if (block == Block::Air) {
                        continue;
                    }

                    bool negZ = chunk.GetBlock(index - Chunk::STRIDE_Z) == Block::Air;
                    bool posZ = chunk.GetBlock(index + Chunk::STRIDE_Z) == Block::Air;
                    bool negX = chunk.GetBlock(index - Chunk::STRIDE_X) == Block::Air;
                    bool posX = chunk.GetBlock(index + Chunk::STRIDE_X) == Block::Air;
                    bool negY = chunk.GetBlock(index - Chunk::STRIDE_Y) == Block::Air;
                    bool posY = chunk.GetBlock(index + Chunk::STRIDE_Y) == Block::Air;

                    if (negZ) AddFace(chunk, index, Face::NegZ, block);
                    if (posZ) AddFace(chunk, index, Face::PosZ, block);
                    if (negX) AddFace(chunk, index, Face::NegX, block);
                    if (posX) AddFace(chunk, index, Face::PosX, block);
                    if (negY) AddFace(chunk, index, Face::NegY, block);
                    if (posY) AddFace(chunk, index, Face::PosY, block);
                }
            }
        }
    }

    void Mesher::MeshExterior(const Chunk& chunk, const Region& region) noexcept {
        for (int y = 1; y < Chunk::HEIGHT - 1; y++) {
            for (int z = 0; z < Chunk::WIDTH; z++) {
                int index = Chunk::ToIndex(0, y, z);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                bool negZ = region.GetBlock(0, y, z - 1) == Block::Air;
                bool posZ = region.GetBlock(0, y, z + 1) == Block::Air;
                bool negX = region.GetBlock(-1, y, z   ) == Block::Air;
                bool posX = chunk.GetBlock(1, y, z     ) == Block::Air;
                bool negY = chunk.GetBlock(0, y - 1, z ) == Block::Air;
                bool posY = chunk.GetBlock(0, y + 1, z ) == Block::Air;

                if (negZ) AddFace(region, 0, y, z, Face::NegZ, block);
                if (posZ) AddFace(region, 0, y, z, Face::PosZ, block);
                if (negX) AddFace(region, 0, y, z, Face::NegX, block);
                if (posX) AddFace(region, 0, y, z, Face::PosX, block);
                if (negY) AddFace(region, 0, y, z, Face::NegY, block);
                if (posY) AddFace(region, 0, y, z, Face::PosY, block);
            }
        }

        for (int y = 1; y < Chunk::HEIGHT - 1; y++) {
            for (int z = 0; z < Chunk::WIDTH; z++) {
                int index = Chunk::ToIndex(Chunk::WIDTH - 1, y, z);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                bool negZ = region.GetBlock(Chunk::WIDTH - 1, y, z - 1) == Block::Air;
                bool posZ = region.GetBlock(Chunk::WIDTH - 1, y, z + 1) == Block::Air;
                bool negX = chunk.GetBlock(Chunk::WIDTH - 2, y, z     ) == Block::Air;
                bool posX = region.GetBlock(Chunk::WIDTH   , y, z     ) == Block::Air;
                bool negY = chunk.GetBlock(Chunk::WIDTH - 1, y - 1, z ) == Block::Air;
                bool posY = chunk.GetBlock(Chunk::WIDTH - 1, y + 1, z ) == Block::Air;

                if (negZ) AddFace(region, Chunk::WIDTH - 1, y, z, Face::NegZ, block);
                if (posZ) AddFace(region, Chunk::WIDTH - 1, y, z, Face::PosZ, block);
                if (negX) AddFace(region, Chunk::WIDTH - 1, y, z, Face::NegX, block);
                if (posX) AddFace(region, Chunk::WIDTH - 1, y, z, Face::PosX, block);
                if (negY) AddFace(region, Chunk::WIDTH - 1, y, z, Face::NegY, block);
                if (posY) AddFace(region, Chunk::WIDTH - 1, y, z, Face::PosY, block);
            }
        }

        for (int y = 1; y < Chunk::HEIGHT - 1; y++) {
            for (int x = 0; x < Chunk::WIDTH; x++) {
                int index = Chunk::ToIndex(x, y, 0);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                bool negZ = region.GetBlock(x, y,   -1 ) == Block::Air;
                bool posZ = chunk.GetBlock(x, y,     1 ) == Block::Air;
                bool negX = region.GetBlock(x - 1, y, 0) == Block::Air;
                bool posX = region.GetBlock(x + 1, y, 0) == Block::Air;
                bool negY = chunk.GetBlock(x, y - 1, 0 ) == Block::Air;
                bool posY = chunk.GetBlock(x, y + 1, 0 ) == Block::Air;

                if (negZ) AddFace(region, x, y, 0, Face::NegZ, block);
                if (posZ) AddFace(region, x, y, 0, Face::PosZ, block);
                if (negX) AddFace(region, x, y, 0, Face::NegX, block);
                if (posX) AddFace(region, x, y, 0, Face::PosX, block);
                if (negY) AddFace(region, x, y, 0, Face::NegY, block);
                if (posY) AddFace(region, x, y, 0, Face::PosY, block);
            }
        }

        for (int y = 1; y < Chunk::HEIGHT - 1; y++) {
            for (int x = 0; x < Chunk::WIDTH; x++) {
                int index = Chunk::ToIndex(x, y, Chunk::WIDTH - 1);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                bool negZ = chunk.GetBlock(x, y, Chunk::WIDTH - 2     ) == Block::Air;
                bool posZ = region.GetBlock(x, y, Chunk::WIDTH        ) == Block::Air;
                bool negX = region.GetBlock(x - 1, y, Chunk::WIDTH - 1) == Block::Air;
                bool posX = region.GetBlock(x + 1, y, Chunk::WIDTH - 1) == Block::Air;
                bool negY = chunk.GetBlock(x, y - 1, Chunk::WIDTH - 1 ) == Block::Air;
                bool posY = chunk.GetBlock(x, y + 1, Chunk::WIDTH - 1 ) == Block::Air;

                if (negZ) AddFace(region, x, y, Chunk::WIDTH - 1, Face::NegZ, block);
                if (posZ) AddFace(region, x, y, Chunk::WIDTH - 1, Face::PosZ, block);
                if (negX) AddFace(region, x, y, Chunk::WIDTH - 1, Face::NegX, block);
                if (posX) AddFace(region, x, y, Chunk::WIDTH - 1, Face::PosX, block);
                if (negY) AddFace(region, x, y, Chunk::WIDTH - 1, Face::NegY, block);
                if (posY) AddFace(region, x, y, Chunk::WIDTH - 1, Face::PosY, block);
            }
        }
    
    }

    void Mesher::MeshTop(const Chunk& chunk, const Region& region) noexcept {
        for (int x = 0; x < Chunk::WIDTH; x++) {
            for (int z = 0; z < Chunk::WIDTH; z++) {
                int index = Chunk::ToIndex(x, Chunk::HEIGHT - 1, z);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                bool negZ = region.GetBlock(x, Chunk::HEIGHT - 1, z - 1) == Block::Air;
                bool posZ = region.GetBlock(x, Chunk::HEIGHT - 1, z + 1) == Block::Air;
                bool negX = region.GetBlock(x - 1, Chunk::HEIGHT - 1, z) == Block::Air;
                bool posX = region.GetBlock(x + 1, Chunk::HEIGHT - 1, z) == Block::Air;
                bool negY = chunk.GetBlock(x, Chunk::HEIGHT - 2, z     ) == Block::Air;

                // No ambient occlusion since it would require blocks above the chunk
                if (negZ) AddFace(index, Face::NegZ, block);
                if (posZ) AddFace(index, Face::PosZ, block);
                if (negX) AddFace(index, Face::NegX, block);
                if (posX) AddFace(index, Face::PosX, block);
                if (negY) AddFace(index, Face::NegY, block);
                AddFace(index, Face::PosY, block); // Always mesh the top face, since no blocks can be above the chunk
            }
        }
    }

    void Mesher::MeshBottom(const Chunk& chunk, const Region& region) noexcept {
        for (int x = 0; x < Chunk::WIDTH; x++) {
            for (int z = 0; z < Chunk::WIDTH; z++) {
                int index = Chunk::ToIndex(x, 0, z);
                Block block = chunk.GetBlock(index);
                if (block == Block::Air) {
                    continue;
                }

                // No ambient occlusion since it would require blocks above the chunk
                bool negZ = region.GetBlock(x, 0, z - 1) == Block::Air;
                bool posZ = region.GetBlock(x, 0, z + 1) == Block::Air;
                bool negX = region.GetBlock(x - 1, 0, z) == Block::Air;
                bool posX = region.GetBlock(x + 1, 0, z) == Block::Air;
                bool posY = chunk.GetBlock(x, 1, z     ) == Block::Air;

                if (negZ) AddFace(index, Face::NegZ, block);
                if (posZ) AddFace(index, Face::PosZ, block);
                if (negX) AddFace(index, Face::NegX, block);
                if (posX) AddFace(index, Face::PosX, block);
                if (posY) AddFace(index, Face::PosY, block);
                // Never mesh the bottom face, since no physics-based player can go below the chunk
            }
        }
    }

    // Adds a face to the mesh without calculating ambient occlusion.
    void Mesher::AddFace(int index, Face face, Block block) noexcept {
        uint8_t texture_index = voxels::world::GetTextureIndex(block, face);

        for (int i = 0; i < 4; i++) {
            uint32_t vertex_data = texture_index << 19;
            vertex_data |= VertexIndex(index) + FACE_VERTEX_OFFSETS[static_cast<int>(face)][i];
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

    // Adds a face to the mesh and calculates ambient occlusion. Must be an interior block.
    void Mesher::AddFace(const Chunk& chunk, int index, Face face, Block block) noexcept {
        AddFace(index, face, block);

        // Apply AO to the last 4 vertices added
        for (int i = 0; i < 4; i++) {
            const auto& offsets = AO_INDICES[static_cast<int>(face)][i];
            bool ao_block_1 = chunk.GetBlock(index + offsets[0]) != Block::Air;
            bool ao_block_2 = chunk.GetBlock(index + offsets[1]) != Block::Air;
            bool ao_block_3 = chunk.GetBlock(index + offsets[2]) != Block::Air;
            int ao_index = static_cast<int>(ao_block_3) << 2 | static_cast<int>(ao_block_2) << 1 | static_cast<int>(ao_block_1);

            vertices_[vertex_count_ - 4 + i] |= AO_LEVELS[ao_index] << 27;
        }
    }

    // Adds a face to the mesh and calculates ambient occlusion. Should be an exterior block.
    void Mesher::AddFace(const Region& region, int x, int y, int z, Face face, Block block) noexcept {
        AddFace(Chunk::ToIndex(x, y, z), face, block);

        // Apply AO to the last 4 vertices added
        for (int i = 0; i < 4; i++) {
            const auto& offsets = AO_OFFSETS[static_cast<int>(face)][i];
            bool ao_block_1 = region.GetBlock(x + offsets[0].x, y + offsets[0].y, z + offsets[0].z) != Block::Air;
            bool ao_block_2 = region.GetBlock(x + offsets[1].x, y + offsets[1].y, z + offsets[1].z) != Block::Air;
            bool ao_block_3 = region.GetBlock(x + offsets[2].x, y + offsets[2].y, z + offsets[2].z) != Block::Air;
            int ao_index = static_cast<int>(ao_block_3) << 2 | static_cast<int>(ao_block_2) << 1 | static_cast<int>(ao_block_1);

            vertices_[vertex_count_ - 4 + i] |= AO_LEVELS[ao_index] << 27;
        }
    }

}
