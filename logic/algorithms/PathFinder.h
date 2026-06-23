#pragma once
/**
 * @file PathFinder.h
 * @brief 路径规划算法
 * @details 实现 Dijkstra（无信息搜索）和 A*（启发式搜索）
 *          用于游戏中的NPC寻路和路线规划
 */

#include <vector>
#include <utility>
#include <functional>
#include <unordered_map>

namespace delivery::algo {

    /**
     * @brief 路径节点
     */
    struct PathNode {
        int x;
        int y;
        int g;                  ///< 起点到当前节点的实际代价
        int h;                  ///< 当前节点到终点的启发式估计
        int f() const { return g + h; }

        bool operator==(const PathNode& other) const {
            return x == other.x && y == other.y;
        }
    };

    /**
     * @brief 路径查找结果
     */
    struct PathResult {
        std::vector<std::pair<int, int>> path;   ///< 路径坐标序列
        int visitedCount = 0;                     ///< 访问节点数
        bool found = false;                      ///< 是否找到路径
    };

    /**
     * @brief 启发式函数类型
     * @param x1,y1 当前坐标
     * @param x2,y2 目标坐标
     * @return 估计代价
     */
    using HeuristicFunc = std::function<int(int, int, int, int)>;

    /**
     * @brief 曼哈顿距离启发式（四方向移动）
     */
    inline int manhattanHeuristic(int x1, int y1, int x2, int y2) {
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }

    /**
     * @brief 欧几里得距离启发式（八方向移动）
     */
    inline int euclideanHeuristic(int x1, int y1, int x2, int y2) {
        int dx = x1 - x2;
        int dy = y1 - y2;
        return static_cast<int>(std::sqrt(dx * dx + dy * dy));
    }

    /**
     * @brief Dijkstra 算法（无信息搜索）
     * @param grid 可通行性检查函数: bool isWalkable(int x, int y)
     * @param startX, startY 起点
     * @param goalX, goalY 终点
     * @return PathResult 包含路径和统计信息
     */
    PathResult dijkstra(
        const std::function<bool(int, int)>& isWalkable,
        int startX, int startY,
        int goalX, int goalY
    );

    /**
     * @brief A* 算法（启发式搜索）
     * @param grid 可通行性检查函数
     * @param startX, startY 起点
     * @param goalX, goalY 终点
     * @param heuristic 启发式函数（默认为曼哈顿距离）
     * @return PathResult 包含路径和统计信息
     */
    PathResult aStar(
        const std::function<bool(int, int)>& isWalkable,
        int startX, int startY,
        int goalX, int goalY,
        HeuristicFunc heuristic = manhattanHeuristic
    );

} // namespace delivery::algo