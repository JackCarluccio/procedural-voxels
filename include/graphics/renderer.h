#pragma once

#include "util/ivec2_hash.h"
#include "world/chunk.h"

#include <unordered_map>

#include <glm/vec2.hpp>

namespace voxels::graphics {
    class Camera;
}

namespace voxels::graphics {

    class Renderer {
    public:
        Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void Init();
        void Draw(const Camera* camera, const std::unordered_map<glm::ivec2, std::unique_ptr<world::Chunk>, voxels::util::IVec2Hash>& chunks);

    };

}
