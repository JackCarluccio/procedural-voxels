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

namespace voxels::input {

class InputManager;

} // namespace voxels::input

namespace voxels::world {

class ChunkManager;

} // namespace voxels::world

namespace voxels::application {

class Application {
public:
    explicit Application(const std::string& title);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void Init();
    void Run();

private:
    int frame_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_frame_time_;
    bool is_running_;

    std::unique_ptr<graphics::Camera> camera_;
    std::unique_ptr<graphics::Window> window_;
    std::unique_ptr<graphics::Renderer> renderer_;
    std::unique_ptr<input::InputManager> input_manager_;
    std::unique_ptr<world::ChunkManager> chunk_manager_;

    void Update(float delta_time);
    void Draw();
};

} // namespace voxels::application

#endif // VOXELS_APPLICATION_APPLICATION_H_
