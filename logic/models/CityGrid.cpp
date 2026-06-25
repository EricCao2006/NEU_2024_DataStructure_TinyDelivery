#include "CityGrid.h"
#include <queue>
#include "../algorithms/PathFinder.h"

namespace delivery {

    CityGrid::CityGrid(const int size) : m_size(size) {}

    bool CityGrid::isValid(const int x, const int y) const {
        return x >= 0 && x < m_size && y >= 0 && y < m_size;
    }

    bool CityGrid::isWalkable(const int x, const int y) const {
        if (!isValid(x, y)) return false;
        return !m_obstacles.contains(encode(x, y));
    }

    std::vector<std::pair<int, int>> CityGrid::neighbors(const int x, const int y) const {
        std::vector<std::pair<int, int>> result;
        for (int i = 0; i < 4; ++i) {
            constexpr int dy[] = {-1, 1, 0, 0};
            constexpr int dx[] = {0, 0, -1, 1};
            int nx = x + dx[i];
            if (int ny = y + dy[i]; isWalkable(nx, ny)) {
                result.emplace_back(nx, ny);
            }
        }
        return result;
    }

    std::vector<std::pair<int, int>> CityGrid::getObstacles() const {
        std::vector<std::pair<int, int>> result;
        for (const int key : m_obstacles) {
            int x = key / m_size;
            int y = key % m_size;
            result.emplace_back(x, y);
        }
        return result;
    }

    void CityGrid::addObstacle(const int x, const int y) {
        if (isValid(x, y)) {
            m_obstacles.insert(encode(x, y));
        }
    }

    void CityGrid::removeObstacle(const int x, const int y) {
        m_obstacles.erase(encode(x, y));
    }

    void CityGrid::clearObstacles() {
        m_obstacles.clear();
    }

    bool CityGrid::isConnected(const int x1, const int y1, const int x2, const int y2) const {
        // 直接使用 PathFinder 检测连通性
        if (!isWalkable(x1, y1) || !isWalkable(x2, y2)) return false;
        if (x1 == x2 && y1 == y2) return true;

        auto isWalkable = [this](const int x, const int y) -> bool {
            return this->isWalkable(x, y);
        };

        const auto result = algo::dijkstra(isWalkable, x1, y1, x2, y2);
        return result.found;
    }
} // namespace delivery