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

        void Update(float delta_time) noexcept;

        bool ShouldClose() const noexcept;

    private:
        GLFWwindow* window_;
        graphics::Camera& camera_;

        float camera_speed_;
        float camera_sensitivity_;

        double last_cursor_x_;
        double last_cursor_y_;

        bool should_close_ = false;

        void ProcessCameraMovement(float delta_time) noexcept;
        void ProcessCameraRotation() noexcept;

        void OnKey(int key, int scancode, int action, int mods) noexcept;

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
    };

}
