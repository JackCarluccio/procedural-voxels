#pragma once

namespace voxels::world::generation {

    class PerlinNoise2D {
    public:
        explicit PerlinNoise2D(float frequency, int octaves, float persistence, float lacunarity);

        float Sample(float x, float y) noexcept;
        // Samples a 2D grid of points corresponding to a chunk of blocks
        void SampleMap(float* map, float x_offset, float y_offset) noexcept;

    private:
        const float frequency_;
        const int octaves_;
        const float persistence_;
        const float lacunarity_;
        const float max_noise_;

    };

}
