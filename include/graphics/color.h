#pragma once

#include <cassert>

namespace voxels::graphics {

    class Color {
    public:
        Color() = default;

        explicit Color(float r, float g, float b, float a = 1.0f) : r_(r), g_(g), b_(b), a_(a) {
            assert(r >= 0.0f && r <= 1.0f && "Color's red component must be in the range [0.0, 1.0]");
            assert(g >= 0.0f && g <= 1.0f && "Color's green component must be in the range [0.0, 1.0]");
            assert(b >= 0.0f && b <= 1.0f && "Color's blue component must be in the range [0.0, 1.0]");
            assert(a >= 0.0f && a <= 1.0f && "Color's alpha component must be in the range [0.0, 1.0]");
        }

        explicit Color(int r, int g, int b, int a = 255)
            : r_(static_cast<float>(r) / 255.0f),
            g_(static_cast<float>(g) / 255.0f),
            b_(static_cast<float>(b) / 255.0f),
            a_(static_cast<float>(a) / 255.0f)
        {
            assert(r >= 0 && r <= 255 && "Color's red component must be in the range [0, 255]");
            assert(g >= 0 && g <= 255 && "Color's green component must be in the range [0, 255]");
            assert(b >= 0 && b <= 255 && "Color's blue component must be in the range [0, 255]");
            assert(a >= 0 && a <= 255 && "Color's alpha component must be in the range [0, 255]");
        }

        [[nodiscard]] float GetR() const noexcept { return r_; }
        [[nodiscard]] float GetG() const noexcept { return g_; }
        [[nodiscard]] float GetB() const noexcept { return b_; }
        [[nodiscard]] float GetA() const noexcept { return a_; }

        void SetR(float r) noexcept {
            assert(r >= 0.0f && r <= 1.0f && "Color's red component must be in the range [0.0, 1.0]");
            r_ = r;
        }

        void SetG(float g) noexcept {
            assert(g >= 0.0f && g <= 1.0f && "Color's green component must be in the range [0.0, 1.0]");
            g_ = g;
        }

        void SetB(float b) noexcept {
            assert(b >= 0.0f && b <= 1.0f && "Color's blue component must be in the range [0.0, 1.0]");
            b_ = b;
        }

        void SetA(float a) noexcept {
            assert(a >= 0.0f && a <= 1.0f && "Color's alpha component must be in the range [0.0, 1.0]");
            a_ = a;
        }

    private:
        float r_ = 0.0f;
        float g_ = 0.0f;
        float b_ = 0.0f;
        float a_ = 1.0f;
    };

}
