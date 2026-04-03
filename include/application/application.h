#ifndef VOXELS_APPLICATION_APPLICATION_H_
#define VOXELS_APPLICATION_APPLICATION_H_

#include <chrono>
#include <memory>
#include <string>

namespace voxels::graphics {

class Camera;
class Renderer;
class Window;

} // namespace voxels::graphics

namespace voxels::application {

class Application {
public:
    explicit Application(int width, int height, const std::string& title);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void Init();
    void Run();

private:
    int frame_;
    std::chrono::steady_clock::time_point last_frame_time_;
    bool is_running_;

    std::unique_ptr<graphics::Camera> camera_;
    std::unique_ptr<graphics::Renderer> renderer_;
    std::unique_ptr<graphics::Window> window_;

    void Update();
    void Draw();
};

} // namespace voxels::application

#endif // VOXELS_APPLICATION_APPLICATION_H_
