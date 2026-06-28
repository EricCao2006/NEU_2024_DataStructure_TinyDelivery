#pragma once

#include "Algorithms.h"

namespace algo {
    /**
     * @brief 路径规划（Dijkstra和A*）
     * @todo 路径规划（Dijkstra和A*）
     */
    class PathFinder : public algo::Algorithm {
    public:
        PathFinder() = default;
        ~PathFinder() = default;

        void findPath();
    };
}