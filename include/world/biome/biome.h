#pragma once

#include "world/biome/temperature.h"
#include "world/biome/humidity.h"

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
