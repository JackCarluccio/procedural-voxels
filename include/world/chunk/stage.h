#pragma once

namespace voxels::world::chunk {

    enum class Stage {
        Empty = 0,
        Shaped,
        Decorated,
        Meshed,
    };

    constexpr Stage NextStage(Stage stage) noexcept {
        switch (stage) {
            case Stage::Empty:
                return Stage::Shaped;
            case Stage::Shaped:
                return Stage::Decorated;
            case Stage::Decorated:
                return Stage::Meshed;
            case Stage::Meshed:
                return Stage::Meshed;
            default:
                return stage;
        }
    }

}
