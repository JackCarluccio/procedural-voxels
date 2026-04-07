#include "world/generation/perlin_noise_2d.h"

#include <cmath>

namespace {

struct Point {
    float x, y;
};

constexpr float Interpolate(float a, float b, float t) noexcept {
    return (b - a) * (3.0f - t * 2.0f) * t * t + a;
}

constexpr Point Gradient(int x, int y) noexcept {
    unsigned int w = 8 * sizeof(unsigned);
    unsigned int s = w / 2;
    unsigned int a = x, b = y;
    a *= 3284157443;

    b ^= a << s | a >> (w - s);
    b *= 1911520717;

    a ^= b << s | b >> (w - s);
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    return Point{
		static_cast<float>(cos(random)),
		static_cast<float>(sin(random))
    };
}

constexpr float DotGridGradient(int cellX, int cellY, float x, float y) noexcept {
    Point gradient = Gradient(cellX, cellY);

    float dx = x - static_cast<float>(cellX);
	float dy = y - static_cast<float>(cellY);

    return dx * gradient.x + dy * gradient.y;
}

} // namespace

namespace voxels::world::generation {

float PerlinNoise2d(float x, float y) noexcept {
    // Surrounding grid points
    int x0 = static_cast<int>(floor(x));
    int y0 = static_cast<int>(floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Interpolation weights
    float tx = x - x0;
    float ty = y - y0;

    // Interpolate between the gradients at the top of the cell
    float n0 = DotGridGradient(x0, y0, x, y);
    float n1 = DotGridGradient(x1, y0, x, y);
    float ix0 = Interpolate(n0, n1, tx);

    // Interpolate between the gradients at the bottom of the cell
    float n2 = DotGridGradient(x0, y1, x, y);
    float n3 = DotGridGradient(x1, y1, x, y);
    float ix1 = Interpolate(n2, n3, tx);

    // Interpolate between the top and bottom gradients
    return Interpolate(ix0, ix1, ty);
}

} // namespace voxels::world::generation
