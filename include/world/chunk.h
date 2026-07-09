#pragma once

#include "graphics/mesh.h"
#include "world/block.h"
#include "world/chunk_stage.h"
#include "world/helper.h"

#include <array>
#include <cstdint>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace voxels::world {

    class Chunk {
    public:
        Chunk() = delete;
        // Intentionally do not initialize blocks_ since the ChunkGenerator will fill it
        explicit Chunk(const glm::ivec2& position)
            { stage_ = ChunkStage::Empty; position_ = position; };

        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        Chunk& operator=(Chunk&&) = delete;

        ChunkStage GetStage() const noexcept { return stage_; }
        void SetStage(ChunkStage stage) noexcept { stage_ = stage; }

        Block GetBlock(int index) const noexcept { return blocks_[index]; }
        
        Block GetBlock(int x, int y, int z) const noexcept {
            return GetBlock(ToIndex(x, y, z));
        }

        Block GetBlock(const glm::ivec3& position) const noexcept {
            return GetBlock(ToIndex(position));
        }

        void SetBlock(int index, Block block) noexcept { blocks_[index] = block; }

        void SetBlock(int x, int y, int z, Block block) noexcept {
            SetBlock(ToIndex(x, y, z), block);
        }

        void SetBlock(const glm::ivec3& position, Block block) noexcept {
            SetBlock(ToIndex(position), block);
        }

        Block* GetBlocksPointer() noexcept { return blocks_.data(); }

        glm::ivec2 GetPosition() const noexcept { return position_; }

        const std::unique_ptr<graphics::Mesh>& GetMesh() const noexcept { return mesh_; }
        void SetMesh(std::unique_ptr<graphics::Mesh> mesh) noexcept { mesh_ = std::move(mesh); }

    private:
        ChunkStage stage_;
        glm::ivec2 position_;
        std::unique_ptr<graphics::Mesh> mesh_;
        std::array<Block, BLOCKS_PER_CHUNK> blocks_;
    };

}
