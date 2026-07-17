#include "world/biomes/biome.h"

namespace voxels::world::biome {

    Biome DetermineBiome(BiomeParameters params) noexcept {
        switch (params.temperature) {
            case Temperature::Cold:
                switch (params.humidity) {
                    case Humidity::Dry: return Biome::Mountains;
                    case Humidity::Humid: return Biome::Tundra;
                }
                break;
            case Temperature::Hot:
                switch (params.humidity) {
                    case Humidity::Dry: return Biome::Desert;
                    case Humidity::Humid: return Biome::Plains;
                }
                break;
        }

        return Biome::Plains; // Should not happen, silences compiler
    }

}
