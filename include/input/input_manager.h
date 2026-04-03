#ifndef VOXELS_INPUT_INPUT_MANAGER_H_
#define VOXELS_INPUT_INPUT_MANAGER_H_

struct GLFWwindow;

namespace voxels::graphics {

class Camera;

} // namespace voxels::graphics

namespace voxels::input {

class InputManager {
public:
    explicit InputManager(GLFWwindow* window, graphics::Camera& camera);

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    InputManager(InputManager&&) = delete;
    InputManager& operator=(InputManager&&) = delete;

    void Update(float delta_time);

private:
    GLFWwindow* window_;
    graphics::Camera& camera_;
};

} // namespace voxels::input

#endif // VOXELS_INPUT_INPUT_MANAGER_H_
