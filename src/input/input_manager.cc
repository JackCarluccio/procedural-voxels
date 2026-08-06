#include "input/input_manager.h"

#include "graphics/camera.h"
#include "graphics/graphics_core.h"

#include <glm/glm.hpp>

namespace voxels::input {

    InputManager::InputManager(const application::Settings& settings, GLFWwindow* window, graphics::Camera& camera)
        : window_(window),
        camera_(camera),
        camera_speed_(settings.user_settings.camera_speed),
        camera_sensitivity_(settings.user_settings.camera_sensitivity)
    {
        glfwGetCursorPos(window_, &last_cursor_x_, &last_cursor_y_);

        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    bool InputManager::Update(float delta_time) noexcept {
        ProcessCameraMovement(delta_time);
        ProcessCameraRotation();

        return CheckForClosure();
    }

    bool InputManager::CheckForClosure() const noexcept {
        return glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    }

    void InputManager::ProcessCameraMovement(float delta_time) noexcept {
        glm::vec3 move_direction(0.0f);
        const glm::vec3 forward(
            cos(camera_.GetPitch()) * cos(camera_.GetYaw()),
            sin(camera_.GetPitch()),
            cos(camera_.GetPitch()) * sin(camera_.GetYaw())
        );
        const glm::vec3 up(0.0f, 1.0f, 0.0f);
        const glm::vec3 right = glm::normalize(glm::cross(forward, up));

        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
            move_direction += forward;
        }
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
            move_direction -= forward;
        }
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
            move_direction -= right;
        }
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
            move_direction += right;
        }
        if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            move_direction -= up;
        }
        if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
            move_direction += up;
        }

        // Normalizing later will give NaN values if the move direction is zero
        if (glm::length(move_direction) <= 0.0f) {
            return;
        }

        move_direction = glm::normalize(move_direction) * (delta_time * camera_speed_);
        camera_.SetPosition(camera_.GetPosition() + move_direction);
    }

    void InputManager::ProcessCameraRotation() noexcept {
        double cursor_x, cursor_y;
        glfwGetCursorPos(window_, &cursor_x, &cursor_y);

        float x_delta = static_cast<float>(cursor_x - last_cursor_x_);
        float y_delta = static_cast<float>(last_cursor_y_ - cursor_y);

        camera_.SetYaw(camera_.GetYaw() + x_delta * camera_sensitivity_);
        camera_.SetPitch(camera_.GetPitch() + y_delta * camera_sensitivity_);

        last_cursor_x_ = cursor_x;
        last_cursor_y_ = cursor_y;
    }

}
