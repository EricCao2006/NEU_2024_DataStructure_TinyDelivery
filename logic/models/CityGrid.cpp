#include "CityGrid.h"
#include <queue>
#include <unordered_map>

namespace delivery {

    CityGrid::CityGrid(int size) : m_size(size) {}

    bool CityGrid::isValid(int x, int y) const {
        return x >= 0 && x < m_size && y >= 0 && y < m_size;
    }

    bool CityGrid::isWalkable(int x, int y) const {
        if (!isValid(x, y)) return false;
        return m_obstacles.find(encode(x, y)) == m_obstacles.end();
    }

    std::vector<std::pair<int, int>> CityGrid::neighbors(int x, int y) const {
        std::vector<std::pair<int, int>> result;
        const int dx[] = {0, 0, -1, 1};
        const int dy[] = {-1, 1, 0, 0};
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (isWalkable(nx, ny)) {
                result.emplace_back(nx, ny);
            }
        }
        return result;
    }

    void CityGrid::addObstacle(int x, int y) {
        if (isValid(x, y)) {
            m_obstacles.insert(encode(x, y));
        }
    }

    void CityGrid::removeObstacle(int x, int y) {
        m_obstacles.erase(encode(x, y));
    }

    void CityGrid::clearObstacles() {
        m_obstacles.clear();
    }

    bool CityGrid::isConnected(int x1, int y1, int x2, int y2) const {
        // TODO: 依赖 PathFinder 模块完成后实现
        // 临时实现：用 BFS 检测连通性
        if (!isWalkable(x1, y1) || !isWalkable(x2, y2)) return false;
        if (x1 == x2 && y1 == y2) return true;

        std::queue<std::pair<int, int>> q;
        std::unordered_map<int, bool> visited;
        q.emplace(x1, y1);
        visited[encode(x1, y1)] = true;

        while (!q.empty()) {
            auto [cx, cy] = q.front();
            q.pop();
            for (auto [nx, ny] : neighbors(cx, cy)) {
                if (nx == x2 && ny == y2) return true;
                int key = encode(nx, ny);
                if (!visited[key]) {
                    visited[key] = true;
                    q.emplace(nx, ny);
                }
            }
        }
        return false;
    }

} // namespace delivery