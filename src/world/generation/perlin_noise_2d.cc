#include "world/generation/perlin_noise_2d.h"

#include "world/helper.h"

#include <cmath>
#include <cstdint>

namespace {

    struct Point {
        float x, y;
    };

    alignas(64)
    constexpr Point gradients[8] = {
        { 1.0f, 0.0f},
        {-1.0f, 0.0f},
        { 0.0f, 1.0f},
        { 0.0f,-1.0f},
        { 0.70710678f, 0.70710678f},
        {-0.70710678f, 0.70710678f},
        { 0.70710678f,-0.70710678f},
        {-0.70710678f,-0.70710678f}
    };

    alignas(64)
    constexpr uint8_t permutation_table[256] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
        140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
        247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
        57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
        60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
        65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
        200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,
        52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,
        207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
        119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
        129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
        218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
        81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,
        184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,
        222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    constexpr float MaxNoise(int octaves, float persistence) noexcept {
        float max_noise = 0.0f;
        float amplitude = 1.0f;
        for (int i = 0; i < octaves; i++) {
            max_noise += amplitude;
            amplitude *= persistence;
        }

        return max_noise;
    }

    constexpr Point Gradient(int x, int y) noexcept {
        uint8_t permutation = permutation_table[(permutation_table[x & 255] + y) & 255];
        return gradients[permutation & 7];
    }

    constexpr float DotGridGradient(int cell_x, int cell_y, float d_x, float d_y) noexcept {
        Point gradient = Gradient(cell_x, cell_y);
        return d_x * gradient.x + d_y * gradient.y;
    }

    float SampleNoise2D(float x, float y) noexcept {
        // Surrounding grid points
        int x_0 = static_cast<int>(floor(x));
        int y_0 = static_cast<int>(floor(y));
        int x_1 = x_0 + 1;
        int y_1 = y_0 + 1;

        // Distance to top-left grid point
        float t_x = x - x_0;
        float t_y = y - y_0;

        // Dotted gradients at the corners of the cell
        float n_0 = DotGridGradient(x_0, y_0, t_x       , t_y       );
        float n_1 = DotGridGradient(x_1, y_0, t_x - 1.0f, t_y       );
        float n_2 = DotGridGradient(x_0, y_1, t_x       , t_y - 1.0f);
        float n_3 = DotGridGradient(x_1, y_1, t_x - 1.0f, t_y - 1.0f);

        // Interpolation weights
        float weight_x = (3.0f - t_x * 2.0f) * t_x * t_x;
        float weight_y = (3.0f - t_y * 2.0f) * t_y * t_y;

        float i_0 = (n_1 - n_0) * weight_x + n_0; // Interpolation between gradients at the top of the cell
        float i_1 = (n_3 - n_2) * weight_x + n_2; // Interpolation between gradients at the bottom of the cell

        // Interpolate between the top and bottom gradients
        return (i_1 - i_0) * weight_y + i_0;
    }

    struct SampleMap2dParams {
        float* map;
        float frequency, amplitude;
        float x_offset, y_offset;
        int index_x_offset, index_y_offset;
        int samples_per_axis;
    };

    void SampleMap2D(const SampleMap2dParams& params) noexcept {
        float cell_origin_x = std::floor(params.x_offset);
        float cell_origin_y = std::floor(params.y_offset);

        float cell_origin_offset_x = params.x_offset - cell_origin_x;
        float cell_origin_offset_y = params.y_offset - cell_origin_y;

        int cell_origin_x_int = static_cast<int>(cell_origin_x);
        int cell_origin_y_int = static_cast<int>(cell_origin_y);

        // All samples are within a single lattice cell, so compute all gradients now
        const Point g_0 = Gradient(cell_origin_x_int    , cell_origin_y_int    );
        const Point g_1 = Gradient(cell_origin_x_int + 1, cell_origin_y_int    );
        const Point g_2 = Gradient(cell_origin_x_int    , cell_origin_y_int + 1);
        const Point g_3 = Gradient(cell_origin_x_int + 1, cell_origin_y_int + 1);

        for (int index_x = 0; index_x < params.samples_per_axis; index_x++) {
            for (int index_y = 0; index_y < params.samples_per_axis; index_y++) {

                // Distance to top-left grid point
                float t_x = cell_origin_offset_x + index_x * params.frequency;
                float t_y = cell_origin_offset_y + index_y * params.frequency;

                // Dotted gradients at the corners of the cell
                float n_0 = (t_x       ) * g_0.x + (t_y       ) * g_0.y;
                float n_1 = (t_x - 1.0f) * g_1.x + (t_y       ) * g_1.y;
                float n_2 = (t_x       ) * g_2.x + (t_y - 1.0f) * g_2.y;
                float n_3 = (t_x - 1.0f) * g_3.x + (t_y - 1.0f) * g_3.y;

                // Interpolation weights
                float weight_x = (3.0f - t_x * 2.0f) * t_x * t_x;
                float weight_y = (3.0f - t_y * 2.0f) * t_y * t_y;

                float i_0 = (n_1 - n_0) * weight_x + n_0; // Interpolation between gradients at the top of the cell
                float i_1 = (n_3 - n_2) * weight_x + n_2; // Interpolation between gradients at the bottom of the cell

                // Interpolate between the top and bottom gradients
                float noise = ((i_1 - i_0) * weight_y + i_0) * params.amplitude;

                params.map[
                    (index_y + params.index_y_offset) + (index_x + params.index_x_offset) * voxels::world::CHUNK_WIDTH
                ] += noise;
            }
        }
    }

}

namespace voxels::world::generation {

    PerlinNoise2D::PerlinNoise2D(float frequency, int octaves, float persistence, float lacunarity)
        : frequency_(frequency),
        octaves_(octaves),
        persistence_(persistence),
        lacunarity_(lacunarity),
        max_noise_(MaxNoise(octaves, persistence))
    {}

    float PerlinNoise2D::Sample(float x, float y) noexcept {
        float noise = 0.0f;
        float frequency = frequency_;
        float amplitude = 1.0f;
        for (int i = 0; i < octaves_; i++) {
            noise += SampleNoise2D(x * frequency, y * frequency) * amplitude;
            frequency *= lacunarity_;
            amplitude *= persistence_;
        }

        return noise / max_noise_;
    }

    void PerlinNoise2D::SampleMap(float* map, float x, float y) noexcept {
        SampleMap2dParams params{
            .map = map,
            .frequency = frequency_,
            .amplitude = 1.0f / max_noise_,
            .x_offset = 0.0f,
            .y_offset = 0.0f,
            .index_x_offset = 0,
            .index_y_offset = 0,
            .samples_per_axis = 0
        };

        for (int i = 0; i < octaves_; i++) {
            // High frequencies may require sampling multiple lattice cells
            int cells_per_axis = static_cast<int>(static_cast<float>(CHUNK_WIDTH) * params.frequency);
            if (cells_per_axis == 0) {
                // Frequency is low enough, sample a single lattice cell
                params.x_offset = x * params.frequency;
                params.y_offset = y * params.frequency;
                params.index_x_offset = 0;
                params.index_y_offset = 0;
                params.samples_per_axis = CHUNK_WIDTH;
                SampleMap2D(params);
            } else {
                // Frequency is too high, so we need to sample multiple lattice cells and stitch them together
                for (int cell_offset_x = 0; cell_offset_x < cells_per_axis; cell_offset_x++) {
                    for (int cell_offset_y = 0; cell_offset_y < cells_per_axis; cell_offset_y++) {
                        params.x_offset = x * params.frequency + cell_offset_x;
                        params.y_offset = y * params.frequency + cell_offset_y;
                        params.index_x_offset = (CHUNK_WIDTH / cells_per_axis) * cell_offset_x;
                        params.index_y_offset = (CHUNK_WIDTH / cells_per_axis) * cell_offset_y;
                        params.samples_per_axis = CHUNK_WIDTH / cells_per_axis;
                        SampleMap2D(params);
                    }
                }
            }

            params.frequency *= lacunarity_;
            params.amplitude *= persistence_;
        }
    }

}
