#ifndef VOXELS_WORLD_HELPER_H_
#define VOXELS_WORLD_HELPER_H_

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace voxels::world {
    constexpr int CHUNK_SIZE = 16;
    constexpr int CHUNK_HEIGHT = 256;

    constexpr int BLOCKS_PER_CHUNK = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;
    constexpr int BLOCKS_PER_CHUNK_SLICE = CHUNK_SIZE * CHUNK_SIZE;
    constexpr int BLOCKS_PER_CHUNK_WALL = CHUNK_SIZE * CHUNK_HEIGHT;
    constexpr int BLOCKS_PER_CHUNK_INTERIOR = (CHUNK_SIZE - 2) * (CHUNK_SIZE - 2) * (CHUNK_HEIGHT - 2);
    constexpr int BLOCKS_PER_CHUNK_EXTERIOR = BLOCKS_PER_CHUNK - BLOCKS_PER_CHUNK_INTERIOR;

    constexpr int CARDINAL_INDEX_OFFSETS[6] = {
        -1,                 // -Z
        +1,                 // +Z
        -CHUNK_SIZE,        // -X
        +CHUNK_SIZE,        // +X
        -BLOCKS_PER_CHUNK_SLICE,  // -Y
        +BLOCKS_PER_CHUNK_SLICE   // +Y
    };

    constexpr glm::ivec3 CARDINAL_VECTOR_OFFSETS[6] = {
        {0, 0, -1}, // -Z
        {0, 0, +1}, // +Z
        {-1, 0, 0}, // -X
        {+1, 0, 0}, // +X
        {0, -1, 0}, // -Y
        {0, +1, 0}  // +Y
    };

    constexpr glm::ivec2 CARDINAL_CHUNK_OFFSETS[4] = {
        glm::ivec2(+0, -1), // -Z
        glm::ivec2(+0, +1), // +Z
        glm::ivec2(-1, +0), // -X
        glm::ivec2(+1, +0)  // +X
    };

    constexpr int ToIndex(int x, int y, int z) {
        return z + x * CHUNK_SIZE + y * BLOCKS_PER_CHUNK_SLICE;
    }

    constexpr int ToIndex(const glm::ivec3& position) {
        return ToIndex(position.x, position.y, position.z);
    }

    constexpr glm::ivec3 ToCell(int index) {
        int z = index % CHUNK_SIZE;
        int x = (index / CHUNK_SIZE) % CHUNK_SIZE;
        int y = index / BLOCKS_PER_CHUNK_SLICE;
        return glm::ivec3(x, y, z);
    }

    constexpr glm::ivec3 ToCell(int x, int y, int z) {
        return glm::ivec3(x, y, z);
    }

    constexpr bool IsOutOfBounds(int x, int y, int z) {
        return
            x < 0 || x >= CHUNK_SIZE ||
            y < 0 || y >= CHUNK_HEIGHT ||
            z < 0 || z >= CHUNK_SIZE;
    }

    constexpr bool IsOutOfBounds(const glm::ivec3& position) {
        return IsOutOfBounds(position.x, position.y, position.z);
    }

    constexpr bool IsExterior(int x, int y, int z) {
        return
            x == 0 || x == CHUNK_SIZE - 1 ||
            y == 0 || y == CHUNK_HEIGHT - 1 ||
            z == 0 || z == CHUNK_SIZE - 1;
    }

    constexpr bool IsExterior(const glm::ivec3& position) {
        return IsExterior(position.x, position.y, position.z);
    }

    constexpr bool IsInterior(int x, int y, int z) {
        return !IsExterior(x, y, z);
    }

    constexpr bool IsInterior(const glm::ivec3& position) {
        return IsInterior(position.x, position.y, position.z);
    }

}

#endif // VOXELS_WORLD_HELPER_H_
