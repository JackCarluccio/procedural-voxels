#ifndef VOXELS_GRAPHICS_RENDERER_H_
#define VOXELS_GRAPHICS_RENDERER_H_

#include "util/ivec2_hash.h"
#include "world/chunk.h"

#include <glm/vec2.hpp>

#include <unordered_map>

namespace voxels::graphics {

class Camera;

} // namespace voxels::graphics

namespace voxels::graphics {

class Renderer {
public:
    explicit Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void Init();
    void Draw(const Camera* camera, const std::unordered_map<glm::ivec2, std::unique_ptr<world::Chunk>, voxels::util::IVec2Hash>& chunks);
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_RENDERER_H_
