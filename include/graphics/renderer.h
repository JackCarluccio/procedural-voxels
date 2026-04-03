#ifndef VOXELS_GRAPHICS_RENDERER_H_
#define VOXELS_GRAPHICS_RENDERER_H_

#include <chrono>

namespace voxels::graphics {

class Camera;

} // namespace voxels::graphics

namespace voxels::graphics {

class Renderer {
public:
    explicit Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void Init();
    void Draw(const Camera* camera);

    int GetFrame() const noexcept { return frame_; }

private:
    int frame_;
    std::chrono::steady_clock::time_point last_frame_time_;
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_RENDERER_H_
