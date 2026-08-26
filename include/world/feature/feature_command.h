#pragma once

#include "world/feature/feature.h"

#include <memory>

namespace voxels::world::feature {

    struct FeatureCommand {
        std::unique_ptr<Feature> feature;
        float average_count;
    };

}
