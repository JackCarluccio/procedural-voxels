#include "world/chunk_generator.h"

#include "world/biomes/biome.h"
#include "world/biomes/biome_data.h"
#include "world/block.h"
#include "world/features/oak_tree.h"
#include "world/generation/linear_spline.h"
#include "world/generation/perlin_noise_2d.h"
#include "world/helper.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace voxels::world {

    namespace {

        constexpr int BASE_HEIGHT = 4;
        constexpr float CONTINENTALNESS_AMPLITUDE = 240.0f;
        generation::PerlinNoise2D continentalness_noise_sampler(1.0f / 512.0f, 6, 0.5f, 2.0f);
        generation::LinearSpline continentalness_spline({
            {-1.00f, 0.00f},
            {-0.15f, 0.40f},
            {-0.10f, 0.40f},
            { 0.10f, 0.60f},
            { 0.15f, 0.60f},
            { 1.00f, 1.00f},
        });

        generation::PerlinNoise2D temperature_noise(1.0f / 512.0f, 4, 0.5f, 2.0f);
        generation::PerlinNoise2D humidity_noise(1.0f / 256.0f, 3, 0.5f, 2.0f);

        // Generates a height map. A height of 0 corresponds to one block of terrain
        void GenerateHeightMap(const Chunk& chunk, uint8_t* height_map) noexcept {
            float chunk_world_x = static_cast<float>(chunk.GetPosition().x * CHUNK_WIDTH);
            float chunk_world_z = static_cast<float>(chunk.GetPosition().y * CHUNK_WIDTH);
            alignas(64) float noise_map[BLOCKS_PER_CHUNK_SLICE] {};
    
            continentalness_noise_sampler.SampleMap(noise_map, chunk_world_x, chunk_world_z);

            for (int i = 0; i < BLOCKS_PER_CHUNK_SLICE; i++) {
                float noise = noise_map[i];
                int height = static_cast<int>(
                    continentalness_spline.GetValue(noise) * CONTINENTALNESS_AMPLITUDE
                ) + BASE_HEIGHT;

                height_map[i] = height;
            }
        }

        void GenerateBiomeMap(const Chunk& chunk, biome::Biome* biome_map) noexcept {
            float chunk_world_x = static_cast<float>(chunk.GetPosition().x * CHUNK_WIDTH);
            float chunk_world_z = static_cast<float>(chunk.GetPosition().y * CHUNK_WIDTH);
            
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    float world_x = chunk_world_x + static_cast<float>(x);
                    float world_z = chunk_world_z + static_cast<float>(z);

                    float temperature = temperature_noise.Sample(world_x, world_z);
                    float humidity = humidity_noise.Sample(world_x, world_z);
                    biome::BiomeParameters biome_params {
                        .temperature = biome::GetTemperatureEnum(temperature),
                        .humidity = biome::GetHumidityEnum(humidity),
                    };

                    biome_map[z + x * CHUNK_WIDTH] = biome::DetermineBiome(biome_params);
                }
            }
        }
    
        void FillChunk(Chunk& chunk, const uint8_t* height_map, const biome::Biome* biome_map) noexcept {
            // Since block layout follows [y][x][z], we can fill all blocks up to the minimum terrain height with stone.
            int min_height = *std::min_element(height_map, height_map + BLOCKS_PER_CHUNK_SLICE);
            std::memset(chunk.GetBlocksPointer(), static_cast<int>(Block::Stone), BLOCKS_PER_CHUNK_SLICE * (min_height + 1 - 4));
    
            /*
                Fill all blocks above the minimum terrain height with air.
                Some blocks will be overwritten with terrain blocks in the next step,
                but this vectorized memset is much faster than a cache-unfriendly loop for each column
            */
            std::memset(
                chunk.GetBlocksPointer() + (min_height + 1) * BLOCKS_PER_CHUNK_SLICE,
                static_cast<int>(Block::Air),
                (CHUNK_HEIGHT - (min_height + 1)) * BLOCKS_PER_CHUNK_SLICE
            );

            // Decorate or somethign
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    int terrainHeight = height_map[z + x * CHUNK_WIDTH];
                    biome::Biome biome = biome_map[z + x * CHUNK_WIDTH];

                    Block surface_block = biome::GetSurfaceBlock(biome);
                    Block subsurface_block = biome::GetSubsurfaceBlock(biome);

                    chunk.SetBlock(x, terrainHeight - 0, z, surface_block);
                    chunk.SetBlock(x, terrainHeight - 1, z, subsurface_block);
                    chunk.SetBlock(x, terrainHeight - 2, z, subsurface_block);
                    chunk.SetBlock(x, terrainHeight - 3, z, subsurface_block);
    
                    // Fill in stone missing from the minimum terrain height to the current terrain height
                    for (int y = min_height - 3; y < terrainHeight - 3; y++) {
                        chunk.SetBlock(x, y, z, Block::Stone);
                    }
                }
            }
        }

    }

    void ChunkGenerator::Shape(Chunk& chunk) noexcept {
        alignas(64) uint8_t height_map[BLOCKS_PER_CHUNK_SLICE];
        alignas(64) biome::Biome biome_map[BLOCKS_PER_CHUNK_SLICE];

        GenerateHeightMap(chunk, height_map);
        GenerateBiomeMap(chunk, biome_map);

        FillChunk(chunk, height_map, biome_map);
    }

    void ChunkGenerator::Decorate(Chunk& chunk, ChunkRegion& chunk_region) const noexcept {
        int surface_level = 0;
        for (int y = CHUNK_HEIGHT - 1; y >= 0; y--) {
            if (chunk.GetBlock(0, y, 0) != Block::Air) {
                surface_level = y;
                break;
            }
        }

        if (features::CanBuildOakTree(chunk_region, 0, surface_level + 1, 0)) {
            features::BuildOakTree(chunk_region, 0, surface_level + 1, 0);
        }
    }

}
