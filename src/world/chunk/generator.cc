#include "world/chunk/generator.h"

#include "util/ivec2_hash.h"
#include "world/biome/biome.h"
#include "world/biome/biome_data.h"
#include "world/block/block.h"
#include "world/generation/linear_spline.h"
#include "world/generation/perlin_noise_2d.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>
#include <vector>

namespace voxels::world::chunk {

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
            float chunk_world_x = static_cast<float>(chunk.GetPosition().x * Chunk::WIDTH);
            float chunk_world_z = static_cast<float>(chunk.GetPosition().y * Chunk::WIDTH);
            alignas(64) float noise_map[Chunk::SLICE_VOLUME] {};
    
            continentalness_noise_sampler.SampleMap(noise_map, chunk_world_x, chunk_world_z);

            for (int i = 0; i < Chunk::SLICE_VOLUME; i++) {
                float noise = noise_map[i];
                int height = static_cast<int>(
                    continentalness_spline.GetValue(noise) * CONTINENTALNESS_AMPLITUDE
                ) + BASE_HEIGHT;

                height_map[i] = height;
            }
        }

        biome::Biome GetBiomeAt(const Chunk& chunk, int x, int z) noexcept {
            float world_x = static_cast<float>(chunk.GetPosition().x * Chunk::WIDTH + x);
            float world_z = static_cast<float>(chunk.GetPosition().y * Chunk::WIDTH + z);

            float temperature = temperature_noise.Sample(world_x, world_z);
            float humidity = humidity_noise.Sample(world_x, world_z);
            biome::BiomeParameters biome_params {
                .temperature = biome::GetTemperatureEnum(temperature),
                .humidity = biome::GetHumidityEnum(humidity),
            };

            return biome::DetermineBiome(biome_params);
        }

        void GenerateBiomeMap(const Chunk& chunk, biome::Biome* biome_map) noexcept {
            for (int x = 0; x < Chunk::WIDTH; x++) {
                for (int z = 0; z < Chunk::WIDTH; z++) {
                    biome_map[z + x * Chunk::WIDTH] = GetBiomeAt(chunk, x, z);
                }
            }
        }
    
        void FillChunk(Chunk& chunk, const uint8_t* height_map) noexcept {
            // Since block layout follows [y][x][z], we can fill all blocks up to the minimum terrain height with stone.
            int min_height = *std::min_element(height_map, height_map + Chunk::SLICE_VOLUME);
            std::memset(chunk.GetBlocksPointer(), static_cast<int>(block::Block::Stone), Chunk::SLICE_VOLUME * (min_height + 1));
    
            // Fill all blocks above the minimum terrain height with air.
            std::memset(
                chunk.GetBlocksPointer() + (min_height + 1) * Chunk::SLICE_VOLUME,
                static_cast<int>(block::Block::Air),
                (Chunk::HEIGHT - (min_height + 1)) * Chunk::SLICE_VOLUME
            );

            for (int x = 0; x < Chunk::WIDTH; x++) {
                for (int z = 0; z < Chunk::WIDTH; z++) {
                    int terrainHeight = height_map[z + x * Chunk::WIDTH];
                    for (int y = min_height + 1; y <= terrainHeight; y++) {
                        chunk.SetBlock(x, y, z, block::Block::Stone);
                    }
                }
            }
        }

        void DecorateSurfaceBlocks(Chunk& chunk, const uint8_t* height_map, const biome::Biome* biome_map) noexcept {
            for (int x = 0; x < Chunk::WIDTH; x++) {
                for (int z = 0; z < Chunk::WIDTH; z++) {
                    int surface_level = height_map[z + x * Chunk::WIDTH];
                    block::Block surface_block = biome::GetSurfaceBlock(biome_map[z + x * Chunk::WIDTH]);
                    block::Block subsurface_block = biome::GetSubsurfaceBlock(biome_map[z + x * Chunk::WIDTH]);
                    chunk.SetBlock(x, surface_level - 0, z, surface_block);
                    chunk.SetBlock(x, surface_level - 1, z, subsurface_block);
                    chunk.SetBlock(x, surface_level - 2, z, subsurface_block);
                    chunk.SetBlock(x, surface_level - 3, z, subsurface_block);
                }
            }
        }

        std::vector<biome::Biome> GetUniqueBiomes(const biome::Biome* biome_map) noexcept {
            std::vector<biome::Biome> unique_biomes;
            for (int i = 0; i < Chunk::SLICE_VOLUME; i++) {
                biome::Biome biome = biome_map[i];
                if (std::find(unique_biomes.begin(), unique_biomes.end(), biome) == unique_biomes.end()) {
                    unique_biomes.push_back(biome);
                }
            }

            return unique_biomes;
        }

        void DecorateFeatures(
            Region& region,
            const uint8_t* height_map,
            const biome::Biome* biome_map,
            const std::vector<biome::Biome>& biomes,
            std::mt19937& rng
        ) noexcept {
            for (auto biome : biomes) {
                const auto& feature_commands = biome::biome_data[static_cast<int>(biome)].feature_commands;
                for (const auto& command : feature_commands) {
                    std::poisson_distribution<int> poisson(command.average_count);
                    int count = poisson(rng);
                    for (int i = 0; i < count; i++) {
                        int x = rng() % Chunk::WIDTH;
                        int z = rng() % Chunk::WIDTH;

                        biome::Biome attempt_biome = biome_map[z + x * Chunk::WIDTH];
                        if (attempt_biome != biome) {
                            continue;
                        }

                        int y = height_map[z + x * Chunk::WIDTH] + 1;
                        if (command.feature->CanGenerate(region, x, y, z)) {
                            command.feature->Generate(region, x, y, z);
                        }
                    }
                }
            }
        }

    }

    void Generator::Init() {
        biome::InitBiomeData();
    }

    void Generator::Shape(Chunk& chunk) noexcept {
        alignas(64) uint8_t height_map[Chunk::SLICE_VOLUME];
        GenerateHeightMap(chunk, height_map);
        FillChunk(chunk, height_map);
    }

    void Generator::Decorate(Chunk& chunk, Region& region) const noexcept {
        std::mt19937 rng;
        rng.seed(util::IVec2Hash{}(chunk.GetPosition()));

        alignas(64) uint8_t height_map[Chunk::SLICE_VOLUME];
        GenerateHeightMap(chunk, height_map);
        
        alignas(64) biome::Biome biome_map[Chunk::SLICE_VOLUME];
        GenerateBiomeMap(chunk, biome_map);

        DecorateSurfaceBlocks(chunk, height_map, biome_map);

        const auto unique_biomes = GetUniqueBiomes(biome_map);
        DecorateFeatures(region, height_map, biome_map, unique_biomes, rng);
    }

}
