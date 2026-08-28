#pragma once

#include "graphics/mesh.h"
#include "util/aabb.h"
#include "world/block/block.h"
#include "world/chunk/stage.h"

#include <array>
#include <cstdint>
#include <optional>
#include <utility>

#include <glm/vec2.hpp>

namespace voxels::world::chunk {

    class Chunk {
    public:
        static constexpr int WIDTH = 16;
        static constexpr int HEIGHT = 256;

        static constexpr int SLICE_VOLUME = WIDTH * WIDTH;
        static constexpr int CHUNK_VOLUME = WIDTH * WIDTH * HEIGHT;

        static constexpr int STRIDE_Z = 1;
        static constexpr int STRIDE_X = WIDTH;
        static constexpr int STRIDE_Y = WIDTH * WIDTH;

        static constexpr int ToIndex(int x, int y, int z) noexcept {
            return z + x * WIDTH + y * WIDTH * WIDTH;
        }

        Chunk() = delete;
        explicit Chunk(const glm::ivec2& position)
            { stage_ = Stage::Empty; position_ = position; };

        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(Chunk&&) = delete;

        Stage GetStage() const noexcept { return stage_; }
        void SetStage(Stage stage) noexcept { stage_ = stage; }

        block::Block GetBlock(int index) const noexcept { return blocks_[index]; }
        block::Block GetBlock(int x, int y, int z) const noexcept { return GetBlock(ToIndex(x, y, z)); }

        void SetBlock(int index, block::Block block) noexcept { blocks_[index] = block; }
        void SetBlock(int x, int y, int z, block::Block block) noexcept { SetBlock(ToIndex(x, y, z), block); }

        block::Block* GetBlocksPointer() noexcept { return blocks_.data(); }

        glm::ivec2 GetPosition() const noexcept { return position_; }

        std::optional<graphics::Mesh>& GetMesh() noexcept { return mesh_; }
        void SetMesh(graphics::Mesh&& mesh) noexcept { mesh_ = std::move(mesh); }

        util::AABB GetAABB() const noexcept {
            const glm::vec3 min = glm::vec3(position_.x * WIDTH, 0.0f, position_.y * WIDTH);
            const glm::vec3 max = glm::vec3((position_.x + 1) * WIDTH, HEIGHT, (position_.y + 1) * WIDTH);
            return util::AABB(min, max);
        }

    private:
        Stage stage_;
        glm::ivec2 position_;
        std::optional<graphics::Mesh> mesh_;
        std::array<block::Block, CHUNK_VOLUME> blocks_;
    };

}
