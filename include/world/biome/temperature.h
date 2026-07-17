#pragma once

namespace voxels::world::biome {

    enum class Temperature {
        Cold,
        Hot,
    };

    constexpr Temperature GetTemperatureEnum(float temperature) noexcept {
        if (temperature < 0.0f) return Temperature::Cold;
        return Temperature::Hot;
    }

}
