#ifndef VOXELS_WORLD_GENERATION_LINEAR_SPLINE_H_
#define VOXELS_WORLD_GENERATION_LINEAR_SPLINE_H_

#include <vector>

namespace voxels::world::generation {

class LinearSpline {
public:
    struct Point {
        float time;
        float value;
    };

    explicit LinearSpline(std::vector<Point> points);
    ~LinearSpline() = default;

    LinearSpline(const LinearSpline&) = default;
    LinearSpline& operator=(const LinearSpline&) = default;
    LinearSpline(LinearSpline&&) = default;
    LinearSpline& operator=(LinearSpline&&) = default;

    float GetValue(float time) const;

private:
    std::vector<Point> points_;
};
    
}

#endif // VOXELS_WORLD_GENERATION_LINEAR_SPLINE_H_
