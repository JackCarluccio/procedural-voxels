#pragma once

#include "application/settings.h"

#include <chrono>
#include <memory>
#include <string>

namespace voxels::graphics {
    class Scene;
    class Renderer;
    class Window;
}

namespace voxels::input {
    class InputManager;
}

namespace voxels::world {
    class ChunkManager;
}

namespace voxels::application {

    class Application {
    public:
        explicit Application(const Settings& settings);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Init();
        void Run();

    private:
        volatile bool is_running_ = false;

        Settings settings_;

        int frame_ = 0;
        std::chrono::steady_clock::time_point start_time_;
        std::chrono::steady_clock::time_point last_frame_time_;
        
        std::unique_ptr<graphics::Scene> scene_;
        std::unique_ptr<graphics::Window> window_;
        std::unique_ptr<graphics::Renderer> renderer_;
        std::unique_ptr<input::InputManager> input_manager_;
        std::unique_ptr<world::ChunkManager> chunk_manager_;

        void Update(float delta_time);
        void Draw();

        void InitGLFW();
        void ConfigureGLFW();
        void CreateWindow();
        void LockCursor();
        void SetVsync();
        void LoadOpenGL();
        void CreateScene();
    };

}
