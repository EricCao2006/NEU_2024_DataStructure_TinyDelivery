#pragma once

#include "Algorithms.h"
#include "../models/CityGrid.h"

#include <vector>
#include <queue>
#include <utility>

namespace algo {

    /**
     * @brief 路径规划（Dijkstra 和 A*）
     * @details 在城市网格地图上寻找最短通行路径
     */
    class PathFinder : public Algorithm {
    public:
        PathFinder() = default;
        std::string algorithmName() const override { return "最短路径(Dijkstra/A*)"; }

        /**
         * @brief Dijkstra 寻路
         * @return 从起点到终点的网格坐标序列（含起终点），无路径则返回空
         */
        std::vector<std::pair<int, int>> findPathDijkstra(
            const models::CityGrid& grid,
            int startX, int startY,
            int endX, int endY);

        /**
         * @brief A* 寻路（曼哈顿距离启发）
         * @return 从起点到终点的网格坐标序列（含起终点），无路径则返回空
         */
        std::vector<std::pair<int, int>> findPathAStar(
            const models::CityGrid& grid,
            int startX, int startY,
            int endX, int endY);

    private:
        struct Node {
            int x, y;
            int gCost = 0;   // 起点到此节点的实际代价
            int fCost = 0;   // g + h
            bool operator>(const Node& o) const { return fCost > o.fCost; }
        };

        // 四方向偏移量：右、左、下、上
        static constexpr int DX[] = {1, -1, 0, 0};
        static constexpr int DY[] = {0, 0, 1, -1};

        std::vector<std::pair<int, int>> reconstructPath(
            const std::vector<std::vector<int>>& parentX,
            const std::vector<std::vector<int>>& parentY,
            int startX, int startY, int endX, int endY);
    };

}
