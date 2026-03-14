#ifndef VOXELS_GRAPHICS_CAMERA_H_
#define VOXELS_GRAPHICS_CAMERA_H_

#include <glm/glm.hpp>

namespace voxels::graphics {

class Camera {
public:
    explicit Camera(float fov, float aspect_ratio, float near_plane, float far_plane);

    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
    Camera(Camera&&) noexcept = default;
    Camera& operator=(Camera&&) noexcept = default;

    glm::mat4 GetViewMatrix() const noexcept;
    glm::mat4 GetProjectionMatrix() const noexcept;

    void SetFov(float fov) noexcept { fov_ = fov; }
    void SetAspectRatio(float aspect_ratio) noexcept { aspect_ratio_ = aspect_ratio; }
    void SetNearPlane(float near_plane) noexcept { near_plane_ = near_plane; }
    void SetFarPlane(float far_plane) noexcept { far_plane_ = far_plane; }

    glm::vec3 GetPosition() noexcept { return position_; }
    float GetPitch() noexcept { return pitch_; }
    float GetYaw() noexcept { return yaw_; }

    void SetPosition(const glm::vec3& position) noexcept { position_ = position; }
    void SetPitch(float pitch) noexcept { pitch_ = pitch; }
    void SetYaw(float yaw) noexcept { yaw_ = yaw; }

private:
    float fov_;
    float aspect_ratio_;
    float near_plane_;
    float far_plane_;

    glm::vec3 position_;
    float pitch_;
    float yaw_;
};

} // namespace voxels::graphics

#endif // VOXELS_GRAPHICS_CAMERA_H_
