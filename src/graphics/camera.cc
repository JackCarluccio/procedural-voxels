#include "graphics/camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace voxels::graphics {

Camera::Camera(float fov, float aspect_ratio, float near_plane, float far_plane)
    : fov_(fov), aspect_ratio_(aspect_ratio), near_plane_(near_plane), far_plane_(far_plane),
      position_(0.0f, 0.0f, 0.0f), pitch_(0.0f), yaw_(0.0f) {}

glm::mat4 Camera::GetViewMatrix() const noexcept {
    float yaw = yaw_ - glm::radians(90.0f); // Adjust yaw to match OpenGL's coordinate system

    glm::vec3 forward;
    forward.x = cos(pitch_) * cos(yaw);
    forward.y = sin(pitch_);
    forward.z = cos(pitch_) * sin(yaw);
    forward = glm::normalize(forward);

    return glm::lookAt(position_, position_ + forward, glm::vec3(0.0f, 1.0f, 0.0f));

}

glm::mat4 Camera::GetProjectionMatrix() const noexcept {
    return glm::perspective(fov_, aspect_ratio_, near_plane_, far_plane_);
}

} // namespace voxels::graphics
