#include "world/generation/linear_spline.h"

#include <algorithm>
#include <stdexcept>

namespace voxels::world::generation {

LinearSpline::LinearSpline(std::vector<Point> points) : points_(std::move(points)) {
    // Ensure the points are sorted by time
    std::sort(points_.begin(), points_.end(), [](const Point& a, const Point& b) {
        return a.time < b.time;
    });

    if (points_.size() < 2) {
        throw std::invalid_argument("At least two points are required to create a LinearSpline.");
    }

    for (int i = 0; i < static_cast<int>(points_.size()) - 1; ++i) {
        if (points_[i].time == points_[i + 1].time) {
            throw std::invalid_argument("Duplicate time values are not allowed in LinearSpline.");
        }
    }
}

float LinearSpline::GetValue(float time) const {
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

} // namespace voxels::world::generation
