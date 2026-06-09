#ifndef VOXELS_WORLD_CHUNK_STAGE_H_
#define VOXELS_WORLD_CHUNK_STAGE_H_

namespace voxels::world {

enum class ChunkStage {
    Empty = 0,
    Shaped,
    Decorated,
    Meshed,
};

}

#endif // VOXELS_WORLD_CHUNK_STAGE_H_
