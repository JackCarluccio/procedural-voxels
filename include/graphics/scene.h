#pragma once

#include "graphics/camera.h"

#include <utility>

namespace voxels::graphics {

    class Scene {
    public:
        Scene() = default;
        explicit Scene(Camera camera) : camera_(std::move(camera)) {}

        Camera& GetCamera() noexcept { return camera_; }
        const Camera& GetCamera() const noexcept { return camera_; }

    private:
        Camera camera_;
    };
    
}
