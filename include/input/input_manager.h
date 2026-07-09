#pragma once

struct GLFWwindow;

namespace voxels::graphics {
    class Camera;
}

namespace voxels::input {

    class InputManager {
    public:
        explicit InputManager(GLFWwindow* window, graphics::Camera& camera);

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        InputManager(InputManager&&) = delete;
        InputManager& operator=(InputManager&&) = delete;

        void Update(float delta_time) noexcept;

    private:
        GLFWwindow* window_;
        graphics::Camera& camera_;

        double last_cursor_x_;
        double last_cursor_y_;
        bool first_mouse_input_;

        void ProcessCameraMovement(float delta_time) noexcept;
        void ProcessCameraRotation() noexcept;
    };

}
