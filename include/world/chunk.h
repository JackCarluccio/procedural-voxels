#ifndef VOXELS_WORLD_CHUNK_H_
#define VOXELS_WORLD_CHUNK_H_

#include "graphics/mesh.h"
#include "world/block.h"

#include <array>
#include <cstdint>
#include <memory>

#include <glm/vec2.hpp>

namespace voxels::world {

class Chunk {
public:
    static constexpr int SIZE = 16;
    static constexpr int HEIGHT = 256;
    static constexpr int BLOCKS_PER_SLICE = SIZE * SIZE;
    static constexpr int BLOCKS_PER_CHUNK = BLOCKS_PER_SLICE * HEIGHT;

    static constexpr int GetIndex(int x, int y, int z) noexcept {
        return z + x * SIZE + y * BLOCKS_PER_SLICE;
    }

    Chunk() = delete;
    explicit Chunk(const glm::ivec2& position);

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;
    Chunk(Chunk&&) = delete;
    Chunk& operator=(Chunk&&) = delete;

    Block GetBlock(int index) const noexcept {
        return blocks_[index];
    }

    Block GetBlock(int x, int y, int z) const noexcept {
        return GetBlock(GetIndex(x, y, z));
    }

    void SetBlock(int index, Block block) noexcept {
        blocks_[index] = block;
    }

    void SetBlock(int x, int y, int z, Block block) noexcept {
        SetBlock(GetIndex(x, y, z), block);
    }

    glm::ivec2 GetPosition() const noexcept {
        return position_;
    }

    const std::unique_ptr<graphics::Mesh>& GetMesh() const noexcept {
        return mesh_;
    }

    void SetMesh(std::unique_ptr<graphics::Mesh> mesh) noexcept {
        mesh_ = std::move(mesh);
    }

private:
    glm::ivec2 position_;
    std::unique_ptr<graphics::Mesh> mesh_;
    std::array<Block, BLOCKS_PER_CHUNK> blocks_;
};

} // namespace voxels::world

#endif // VOXELS_WORLD_CHUNK_H_
