#pragma once

#include <vector>
#include <utility>
#include <unordered_set>

namespace delivery {

    /**
     * @brief 城市网格类，管理地图和障碍物
     */
    class CityGrid {
    public:
        CityGrid() = default;
        explicit CityGrid(int size);

        // ========== Getter ==========
        int size() const { return m_size; }

        /**
         * @brief 检查坐标是否在网格内
         */
        bool isValid(int x, int y) const;

        /**
         * @brief 检查坐标是否可通行（无障碍物）
         */
        bool isWalkable(int x, int y) const;

        /**
         * @brief 获取某点的邻居坐标（上下左右）
         * @return 可通行的邻居坐标列表
         */
        std::vector<std::pair<int, int>> neighbors(int x, int y) const;

        /**
         * @brief 获取所有障碍物坐标列表
         */
        std::vector<std::pair<int, int>> getObstacles() const;

        // ========== Setter ==========
        void setSize(const int size) { m_size = size; }
        void addObstacle(int x, int y);
        void removeObstacle(int x, int y);
        void clearObstacles();

        /**
         * @brief 判断两点之间是否连通
         */
        bool isConnected(int x1, int y1, int x2, int y2) const;

    private:
        int m_size = 20;
        std::unordered_set<int> m_obstacles;                        ///< 障碍物集合，编码为 x * size + y

        /**
         * @brief 将坐标编码为唯一整数
         */
        int encode(const int x, const int y) const { return x * m_size + y; }
    };

} // namespace delivery