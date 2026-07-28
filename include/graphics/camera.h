#pragma once

#include <glm/glm.hpp>

namespace voxels::graphics {

    class Camera {
    public:
        Camera() = default;
        explicit Camera(float fov, float aspect_ratio, float near_plane, float far_plane);

        glm::mat4 GetViewMatrix() const noexcept;
        glm::mat4 GetProjectionMatrix() const noexcept;

        void SetFov(float fov) noexcept { fov_ = fov; }
        void SetAspectRatio(float aspect_ratio) noexcept { aspect_ratio_ = aspect_ratio; }
        void SetNearPlane(float near_plane) noexcept { near_plane_ = near_plane; }
        void SetFarPlane(float far_plane) noexcept { far_plane_ = far_plane; }

        glm::vec3 GetPosition() const noexcept { return position_; }
        float GetPitch() const noexcept { return pitch_; }
        float GetYaw() const noexcept { return yaw_; }

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

}
