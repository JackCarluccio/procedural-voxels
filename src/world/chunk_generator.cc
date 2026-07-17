#include "world/chunk_generator.h"

#include "util/ivec2_hash.h"
#include "world/biome/biome.h"
#include "world/biome/biome_data.h"
#include "world/block.h"
#include "world/generation/linear_spline.h"
#include "world/generation/perlin_noise_2d.h"
#include "world/helper.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <random>
#include <vector>

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

        biome::Biome GetBiomeAt(const Chunk& chunk, int x, int z) noexcept {
            float world_x = static_cast<float>(chunk.GetPosition().x * CHUNK_WIDTH + x);
            float world_z = static_cast<float>(chunk.GetPosition().y * CHUNK_WIDTH + z);

            float temperature = temperature_noise.Sample(world_x, world_z);
            float humidity = humidity_noise.Sample(world_x, world_z);
            biome::BiomeParameters biome_params {
                .temperature = biome::GetTemperatureEnum(temperature),
                .humidity = biome::GetHumidityEnum(humidity),
            };

            return biome::DetermineBiome(biome_params);
        }

        void GenerateBiomeMap(const Chunk& chunk, biome::Biome* biome_map) noexcept {
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    biome_map[z + x * CHUNK_WIDTH] = GetBiomeAt(chunk, x, z);
                }
            }
        }
    
        void FillChunk(Chunk& chunk, const uint8_t* height_map) noexcept {
            // Since block layout follows [y][x][z], we can fill all blocks up to the minimum terrain height with stone.
            int min_height = *std::min_element(height_map, height_map + BLOCKS_PER_CHUNK_SLICE);
            std::memset(chunk.GetBlocksPointer(), static_cast<int>(Block::Stone), BLOCKS_PER_CHUNK_SLICE * (min_height + 1));
    
            // Fill all blocks above the minimum terrain height with air.
            std::memset(
                chunk.GetBlocksPointer() + (min_height + 1) * BLOCKS_PER_CHUNK_SLICE,
                static_cast<int>(Block::Air),
                (CHUNK_HEIGHT - (min_height + 1)) * BLOCKS_PER_CHUNK_SLICE
            );

            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    int terrainHeight = height_map[z + x * CHUNK_WIDTH];
                    for (int y = min_height + 1; y <= terrainHeight; y++) {
                        chunk.SetBlock(x, y, z, Block::Stone);
                    }
                }
            }
        }

        void DecorateSurfaceBlocks(Chunk& chunk, const uint8_t* height_map, const biome::Biome* biome_map) noexcept {
            for (int x = 0; x < CHUNK_WIDTH; x++) {
                for (int z = 0; z < CHUNK_WIDTH; z++) {
                    int surface_level = height_map[z + x * CHUNK_WIDTH];
                    Block surface_block = biome::GetSurfaceBlock(biome_map[z + x * CHUNK_WIDTH]);
                    Block subsurface_block = biome::GetSubsurfaceBlock(biome_map[z + x * CHUNK_WIDTH]);
                    chunk.SetBlock(x, surface_level - 0, z, surface_block);
                    chunk.SetBlock(x, surface_level - 1, z, subsurface_block);
                    chunk.SetBlock(x, surface_level - 2, z, subsurface_block);
                    chunk.SetBlock(x, surface_level - 3, z, subsurface_block);
                }
            }
        }

        std::vector<biome::Biome> GetUniqueBiomes(const biome::Biome* biome_map) noexcept {
            std::vector<biome::Biome> unique_biomes;
            for (int i = 0; i < BLOCKS_PER_CHUNK_SLICE; i++) {
                biome::Biome biome = biome_map[i];
                if (std::find(unique_biomes.begin(), unique_biomes.end(), biome) == unique_biomes.end()) {
                    unique_biomes.push_back(biome);
                }
            }

            return unique_biomes;
        }

        void DecorateFeatures(
            ChunkRegion& chunk_region,
            const uint8_t* height_map,
            const biome::Biome* biome_map,
            const std::vector<biome::Biome>& biomes,
            std::mt19937& rng
        ) noexcept {
            for (auto biome : biomes) {
                const auto& feature_commands = biome::biome_data[static_cast<int>(biome)].feature_commands;
                for (const auto& command : feature_commands) {
                    for (int i = 0; i < command.attempts; i++) {
                        int x = rng() % CHUNK_WIDTH;
                        int z = rng() % CHUNK_WIDTH;

                        biome::Biome attempt_biome = biome_map[z + x * CHUNK_WIDTH];
                        if (attempt_biome != biome) {
                            continue;
                        }

                        int y = height_map[z + x * CHUNK_WIDTH] + 1;
                        if (command.feature->CanGenerate(chunk_region, x, y, z)) {
                            command.feature->Generate(chunk_region, x, y, z);
                        }
                    }
                }
            }
        }

    }

    void ChunkGenerator::Init() {
        biome::InitBiomeData();
    }

    void ChunkGenerator::Shape(Chunk& chunk) noexcept {
        alignas(64) uint8_t height_map[BLOCKS_PER_CHUNK_SLICE];
        GenerateHeightMap(chunk, height_map);
        FillChunk(chunk, height_map);
    }

    void ChunkGenerator::Decorate(Chunk& chunk, ChunkRegion& chunk_region) const noexcept {
        std::mt19937 rng;
        rng.seed(util::IVec2Hash{}(chunk.GetPosition()));

        alignas(64) uint8_t height_map[BLOCKS_PER_CHUNK_SLICE];
        GenerateHeightMap(chunk, height_map);
        
        alignas(64) biome::Biome biome_map[BLOCKS_PER_CHUNK_SLICE];
        GenerateBiomeMap(chunk, biome_map);

        DecorateSurfaceBlocks(chunk, height_map, biome_map);

        const auto unique_biomes = GetUniqueBiomes(biome_map);
        DecorateFeatures(chunk_region, height_map, biome_map, unique_biomes, rng);
    }

}
