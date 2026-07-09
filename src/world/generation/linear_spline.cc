#include "world/generation/linear_spline.h"

#include <algorithm>
#include <cassert>

namespace voxels::world::generation {

    LinearSpline::LinearSpline(std::vector<Point> points) : points_(std::move(points)) {
        assert(!points_.empty() && "LinearSpline must have at least one point.");

        // Points must be sorted chronologically
        std::sort(points_.begin(), points_.end(), [](const Point& a, const Point& b) {
            return a.time < b.time;
        });


        for (int i = 0; i < static_cast<int>(points_.size()) - 1; i++) {
            assert(points_[i].time != points_[i + 1].time && "LinearSpline points must have unique time values.");
        }
    }

    float LinearSpline::GetValue(float time) const noexcept {
        if (time <= points_[0].time) {
            return points_[0].value;
        }

        for (int i = 1; i < static_cast<int>(points_.size()); i++) {
            if (points_[i].time > time) {
                const Point& previous = points_[i - 1];
                const Point& current = points_[i];
                float t = (time - previous.time) / (current.time - previous.time);
                return previous.value + t * (current.value - previous.value);
            }
        }

        return points_.back().value;
    }

}
