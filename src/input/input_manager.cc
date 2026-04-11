#include "input/input_manager.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"

#include <glm/glm.hpp>

namespace voxels::input {

InputManager::InputManager(GLFWwindow* window, graphics::Camera& camera) : window_(window), camera_(camera) {
    glfwGetCursorPos(window_, &last_cursor_x_, &last_cursor_y_);
    first_mouse_input_ = true;
}

void InputManager::Update(float delta_time) {
    ProcessCameraMovement(delta_time);
    ProcessCameraRotation();
}

void InputManager::ProcessCameraMovement(float delta_time) {
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

    move_direction = glm::normalize(move_direction) * (delta_time * 96.0f);
    camera_.SetPosition(camera_.GetPosition() + move_direction);
}

void InputManager::ProcessCameraRotation() {
    double cursor_x, cursor_y;
    glfwGetCursorPos(window_, &cursor_x, &cursor_y);

    float x_delta = static_cast<float>(cursor_x - last_cursor_x_);
    float y_delta = static_cast<float>(last_cursor_y_ - cursor_y);

    camera_.SetYaw(camera_.GetYaw() + x_delta * 0.0015f);
    camera_.SetPitch(camera_.GetPitch() + y_delta * 0.0015f);

    last_cursor_x_ = cursor_x;
    last_cursor_y_ = cursor_y;
}

} // namespace voxels::input
