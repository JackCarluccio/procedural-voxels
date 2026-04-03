#ifndef VOXELS_GRAPHICS_RENDERER_H_
#define VOXELS_GRAPHICS_RENDERER_H_

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
    void Draw(const Camera* camera);
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_RENDERER_H_
