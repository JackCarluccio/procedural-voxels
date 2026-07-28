#pragma once

#include "graphics/camera.h"
#include "graphics/skybox.h"

#include <utility>

namespace voxels::graphics {

    class Scene {
    public:
        Scene() = default;
        explicit Scene(Camera&& camera, Skybox&& skybox)
            : camera_(std::move(camera)),
            skybox_(std::move(skybox))
        {}

        [[nodiscard]] Camera& GetCamera() noexcept { return camera_; }
        [[nodiscard]] const Camera& GetCamera() const noexcept { return camera_; }

        [[nodiscard]] Skybox& GetSkybox() noexcept { return skybox_; }
        [[nodiscard]] const Skybox& GetSkybox() const noexcept { return skybox_; }

    private:
        Camera camera_;
        Skybox skybox_;
    };
    
}
