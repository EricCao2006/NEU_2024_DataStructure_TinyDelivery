#pragma once

#include <cmath>
#include <algorithm>

namespace utils {

    inline int manhattanDistance(const int x1, const int y1, const int x2, const int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    inline double euclideanDistance(const int x1, const int y1, const int x2, const int y2) {
        const int dx = x1 - x2;
        const int dy = y1 - y2;
        return std::sqrt(dx * dx + dy * dy);
    }

    inline double clamp(const double val, const double min, const double max) {
        return std::max(min, std::min(max, val));
    }

    inline int clamp(const int val, const int min, const int max) {
        return std::max(min, std::min(max, val));
    }

    template<typename T>
    T lerp(T a, T b, double t) {
        return a + (b - a) * t;
    }

}