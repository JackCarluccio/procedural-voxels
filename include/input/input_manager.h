#pragma once

#include "application/settings.h"

struct GLFWwindow;

namespace voxels::graphics {
    class Camera;
}

namespace voxels::input {

    class InputManager {
    public:
        explicit InputManager(const application::Settings& settings, GLFWwindow* window, graphics::Camera& camera);

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        bool Update(float delta_time) noexcept;

    private:
        GLFWwindow* window_;
        graphics::Camera& camera_;

        float camera_speed_;
        float camera_sensitivity_;

        double last_cursor_x_;
        double last_cursor_y_;

        bool CheckForClosure() const noexcept;
        void ProcessCameraMovement(float delta_time) noexcept;
        void ProcessCameraRotation() noexcept;
    };

}
