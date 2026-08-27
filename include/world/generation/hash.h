#pragma once

#include <cstdint>

namespace voxels::world::generation::hash {

    inline uint32_t WORLD_SEED = 0xDEADBEEF;

    inline uint32_t FeatureSeed() noexcept {
        constexpr uint32_t FEATURE_MASK = 0x000000FF;
        return WORLD_SEED & FEATURE_MASK;
    }

    // Squirrel5 hash function
    constexpr uint32_t Hash(uint32_t input, uint32_t seed) noexcept {
        constexpr uint32_t SQ5_BIT_NOISE1 = 0b11010010101010000000101000111111;
        constexpr uint32_t SQ5_BIT_NOISE2 = 0b10101000100001001111000110010111;
        constexpr uint32_t SQ5_BIT_NOISE3 = 0b01101100011100110110111101001011;
        constexpr uint32_t SQ5_BIT_NOISE4 = 0b10110111100111110011101010111011;
        constexpr uint32_t SQ5_BIT_NOISE5 = 0b00011011010101101100010011110101;

        uint32_t mangledBits = input;
        mangledBits *= SQ5_BIT_NOISE1;
        mangledBits += seed;
        mangledBits ^= mangledBits >> 9;
        mangledBits += SQ5_BIT_NOISE2;
        mangledBits ^= mangledBits >> 11;
        mangledBits *= SQ5_BIT_NOISE3;
        mangledBits ^= mangledBits >> 13;
        mangledBits += SQ5_BIT_NOISE4;
        mangledBits ^= mangledBits >> 15;
        mangledBits *= SQ5_BIT_NOISE5;
        mangledBits ^= mangledBits >> 17;

        return mangledBits;
    }

    inline uint32_t FeatureHash(int x) noexcept {
        return Hash(static_cast<uint32_t>(x), FeatureSeed());
    }

    inline uint32_t FeatureHash(int x, int y) noexcept {
        constexpr int PRIME1 = 198491317;
        return Hash(static_cast<uint32_t>(x + PRIME1 * y), FeatureSeed());
    }

    inline uint32_t FeatureHash(int x, int y, int z) noexcept {
        constexpr int PRIME1 = 198491317;
        constexpr int PRIME2 = 6542989;
        return Hash(static_cast<uint32_t>(x + PRIME1 * y + PRIME2 * z), FeatureSeed());
    }

}
