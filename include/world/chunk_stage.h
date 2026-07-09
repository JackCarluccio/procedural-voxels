#pragma once

namespace voxels::world {

    enum class ChunkStage {
        Empty = 0,
        Shaped,
        Decorated,
        Meshed,
    };

}
