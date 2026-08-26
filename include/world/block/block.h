#pragma once

#include <cstdint>
#include <string>

namespace voxels::world::block {

    constexpr int COUNT = 10;

    enum class Block : uint8_t {
        Air = 0,
        Stone,
        Dirt,
        Grass,
        OakLog,
        OakLeaves,
        Sand,
        SandStone,
        Snow,
        SnowyDirt,
    };

    constexpr std::string ToString(Block block) {
        switch (block) {
            case Block::Air: return "Air";
            case Block::Stone: return "Stone";
            case Block::Dirt: return "Dirt";
            case Block::Grass: return "Grass";
            case Block::OakLog: return "OakLog";
            case Block::OakLeaves: return "OakLeaves";
            case Block::Sand: return "Sand";
            case Block::SandStone: return "SandStone";
            case Block::Snow: return "Snow";
            case Block::SnowyDirt: return "SnowyDirt";
            default: return "Unknown";
        }
    }

}
