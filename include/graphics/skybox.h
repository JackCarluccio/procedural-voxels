#pragma once

#include "graphics/color.h"

namespace voxels::graphics {

    class Skybox {
    public:
        Skybox() = default;
        explicit Skybox(Color color) : color_(color) {}

        [[nodiscard]] Color GetColor() const noexcept { return color_; }
        void SetColor(Color color) noexcept { color_ = color; }

    private:
        Color color_;
    };
}
