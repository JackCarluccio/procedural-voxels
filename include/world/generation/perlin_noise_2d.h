#ifndef VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H
#define VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H

namespace voxels::world::generation {

class PerlinNoise2d {
public:
    explicit PerlinNoise2d(float frequency, int octaves, float persistence, float lacunarity);

    float Sample(float x, float y) const noexcept;

private:
    float frequency_;
    int octaves_;
    float persistence_;
    float lacunarity_;
    float max_noise_;

};

}

#endif // VOXELS_WORLD_GENERATION_PERLIN_NOISE_2D_H
