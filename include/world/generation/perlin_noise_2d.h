#ifndef VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H
#define VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H

namespace voxels::world::generation {

class PerlinNoise2d {
public:
    explicit PerlinNoise2d(float frequency, int octaves, float persistence, float lacunarity);

    PerlinNoise2d(const PerlinNoise2d&) = default;
    PerlinNoise2d& operator=(const PerlinNoise2d&) = default;
    PerlinNoise2d(PerlinNoise2d&&) = default;
    PerlinNoise2d& operator=(PerlinNoise2d&&) = default;

    float Sample(float x, float y) const noexcept;
    void SampleMap(float* map, float x_offset, float y_offset) const noexcept;

private:
    const float frequency_;
    const int octaves_;
    const float persistence_;
    const float lacunarity_;
    const float max_noise_;

};

}

#endif // VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H
