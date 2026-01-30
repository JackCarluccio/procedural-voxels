#pragma once

#include "graphics/window.h"

#include <memory>
#include <string>

namespace voxels::application {

    class Application {
    public:
        explicit Application(int width, int height, const std::string& title);
        ~Application();

        // The application should not be copied or moved
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void Run();

    private:
        std::unique_ptr<graphics::Window> window_;
        bool is_running_;

        void Update();
        void Draw();
    };

} // namespace voxels::application