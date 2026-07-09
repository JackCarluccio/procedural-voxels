#pragma once

#include <vector>

namespace voxels::world::generation {

    class LinearSpline {
    public:
        struct Point {
            float time;
            float value;
        };

        explicit LinearSpline(std::vector<Point> points);

        float GetValue(float time) const noexcept;

    private:
        std::vector<Point> points_;
    };
    
}
