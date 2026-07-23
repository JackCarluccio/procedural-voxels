#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace voxels::world {
    constexpr int CHUNK_WIDTH = 16;
    constexpr int CHUNK_HEIGHT = 256;

    constexpr int BLOCKS_PER_CHUNK = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;
    constexpr int BLOCKS_PER_CHUNK_SLICE = CHUNK_WIDTH * CHUNK_WIDTH;
    constexpr int BLOCKS_PER_CHUNK_WALL = CHUNK_WIDTH * CHUNK_HEIGHT;
    constexpr int BLOCKS_PER_CHUNK_INTERIOR = (CHUNK_WIDTH - 2) * (CHUNK_WIDTH - 2) * (CHUNK_HEIGHT - 2);
    constexpr int BLOCKS_PER_CHUNK_EXTERIOR = BLOCKS_PER_CHUNK - BLOCKS_PER_CHUNK_INTERIOR;

    constexpr int STRIDE_Z = 1;
    constexpr int STRIDE_X = CHUNK_WIDTH;
    constexpr int STRIDE_Y = BLOCKS_PER_CHUNK_SLICE;

    constexpr int BLOCK_INDEX_OFFSETS[6] = {
        -STRIDE_Z,
        +STRIDE_Z,
        -STRIDE_X,
        +STRIDE_X,
        -STRIDE_Y,
        +STRIDE_Y,
    };
    constexpr glm::ivec3 BLOCK_VECTOR_OFFSETS[6] = {
        { 0,  0, -1},
        { 0,  0,  1},
        {-1,  0,  0},
        { 1,  0,  0},
        { 0, -1,  0},
        { 0,  1,  0},
    };

    constexpr glm::ivec2 CHUNK_OFFSETS[8] = {
        glm::ivec2( 1,  0),
        glm::ivec2( 1,  1),
        glm::ivec2( 0,  1),
        glm::ivec2(-1,  1),
        glm::ivec2(-1,  0),
        glm::ivec2(-1, -1),
        glm::ivec2( 0, -1),
        glm::ivec2( 1, -1),
    };

    constexpr int ToIndex(int x, int y, int z) noexcept {
        return z + x * STRIDE_X + y * STRIDE_Y;
    }

    constexpr int ToIndex(const glm::ivec3& position) noexcept {
        return ToIndex(position.x, position.y, position.z);
    }

    constexpr glm::ivec3 ToCell(int index) noexcept {
        int z = index % STRIDE_X;
        int x = (index % STRIDE_Y) / STRIDE_X;
        int y = index / STRIDE_Y;
        return glm::ivec3(x, y, z);
    }

    constexpr glm::ivec3 ToCell(int x, int y, int z) noexcept {
        return glm::ivec3(x, y, z);
    }

    constexpr bool IsOutOfBounds(int x, int y, int z) noexcept {
        return
            x < 0 || x >= CHUNK_WIDTH ||
            y < 0 || y >= CHUNK_HEIGHT ||
            z < 0 || z >= CHUNK_WIDTH;
    }

    constexpr bool IsOutOfBounds(const glm::ivec3& position) noexcept {
        return IsOutOfBounds(position.x, position.y, position.z);
    }

    constexpr bool IsExterior(int x, int y, int z) noexcept {
        return
            x == 0 || x == CHUNK_WIDTH - 1 ||
            y == 0 || y == CHUNK_HEIGHT - 1 ||
            z == 0 || z == CHUNK_WIDTH - 1;
    }

    constexpr bool IsExterior(const glm::ivec3& position) noexcept {
        return IsExterior(position.x, position.y, position.z);
    }

    constexpr bool IsInterior(int x, int y, int z) noexcept {
        return !IsExterior(x, y, z);
    }

    constexpr bool IsInterior(const glm::ivec3& position) noexcept {
        return IsInterior(position.x, position.y, position.z);
    }

}
