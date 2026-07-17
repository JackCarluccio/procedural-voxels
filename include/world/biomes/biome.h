#pragma once

#include "world/biomes/temperature.h"
#include "world/biomes/humidity.h"
#include "world/block.h"

namespace voxels::world::biome {

    enum class Biome {
        Plains,
        Desert,
        Tundra,
        Mountains,
    };

    struct BiomeParameters {
        Temperature temperature;
        Humidity humidity;
    };
    Biome DetermineBiome(BiomeParameters) noexcept;

}
