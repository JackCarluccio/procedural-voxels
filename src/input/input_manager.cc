#include "input/input_manager.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"

#include <glm/glm.hpp>

namespace voxels::input {

InputManager::InputManager(GLFWwindow* window, graphics::Camera& camera) : window_(window), camera_(camera) { }

void InputManager::Update(float delta_time) {
    glm::vec3 move_direction(0.0f);

    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        move_direction.z -= 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        move_direction.z += 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        move_direction.x -= 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        move_direction.x += 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        move_direction.y -= 1.0f;
    }
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
        move_direction.y += 1.0f;
    }

    // Normalizing later will give NaN values if the move direction is zero
    if (glm::length(move_direction) <= 0.0f) {
        return;
    }

    move_direction = glm::normalize(move_direction) * (delta_time * 5.0f);
    camera_.SetPosition(camera_.GetPosition() + move_direction);
}

} // namespace voxels::input
