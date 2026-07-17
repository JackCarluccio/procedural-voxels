#pragma once

namespace voxels::world::biome {

    enum class Humidity {
        Dry,
        Humid,
    };

    constexpr Humidity GetHumidityEnum(float humidity) noexcept {
        if (humidity <= 0.0f) return Humidity::Dry;
        return Humidity::Humid;
    }
}
