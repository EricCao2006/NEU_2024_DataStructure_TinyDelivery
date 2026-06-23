#pragma once

#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>

namespace delivery::utils {

    inline int manhattanDistance(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    inline double euclideanDistance(int x1, int y1, int x2, int y2) {
        int dx = x1 - x2;
        int dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }

    inline double clamp(double val, double min, double max) {
        return std::max(min, std::min(max, val));
    }

    inline int clamp(int val, int min, int max) {
        return std::max(min, std::min(max, val));
    }

    template<typename T>
    inline T lerp(T a, T b, double t) {
        return a + (b - a) * t;
    }

} // namespace delivery::utils