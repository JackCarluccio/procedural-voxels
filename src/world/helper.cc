#include "world/helper.h"

namespace voxels::world {

int INNER_INDICES[BLOCKS_PER_CHUNK_INTERIOR];

void helper::Init() noexcept {
    int i = 0;
    for (int y = 0; y < CHUNK_HEIGHT; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (IsInterior(x, y, z)) {
                    INNER_INDICES[i++] = ToIndex(x, y, z);
                }
            }
        }
    }
}

} // namespace voxels::world::helper
