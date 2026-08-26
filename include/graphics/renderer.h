#pragma once

#include "application/settings.h"
#include "graphics/scene.h"
#include "util/ivec2_hash.h"

#include <memory>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace voxels::world::chunk {
    class Chunk;
}

namespace voxels::graphics {

    class Renderer {
    public:
        Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void Init(const application::Settings& settings, unsigned char* texture_data);
        void Draw(const Scene& scene, const std::unordered_map<glm::ivec2, std::unique_ptr<world::chunk::Chunk>, voxels::util::IVec2Hash>& chunks);

    };

}
