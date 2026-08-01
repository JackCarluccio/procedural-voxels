#pragma once

#include "application/settings.h"
#include "graphics/scene.h"
#include "util/ivec2_hash.h"
#include "world/chunk.h"

#include <memory>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace voxels::graphics {

    class Renderer {
    public:
        Renderer() = default;

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void Init(const application::Settings& settings);
        void Draw(const Scene& scene, const std::unordered_map<glm::ivec2, std::unique_ptr<world::Chunk>, voxels::util::IVec2Hash>& chunks);

    };

}
