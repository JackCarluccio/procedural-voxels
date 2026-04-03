#include "input/input_manager.h"

#include "graphics/graphics_core.h"
#include "graphics/camera.h"

namespace voxels::input {

InputManager::InputManager(GLFWwindow* window, graphics::Camera& camera) : window_(window), camera_(camera) { }

void InputManager::Update(float delta_time) {
    
}

} // namespace voxels::input
