#pragma once

namespace voxels::world::chunk {

    enum class Stage {
        Empty = 0,
        Shaped,
        Decorated,
        Meshed,
    };

}
