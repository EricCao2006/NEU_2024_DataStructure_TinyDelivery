#include "PathFinder.h"
#include <queue>
#include <unordered_set>
#include <algorithm>

namespace delivery::algo {

// ========== 辅助结构 ==========

struct PathState {
    int x, y;
    int g;
    int h;
    int parentX, parentY;

    [[nodiscard]] int f() const { return g + h; }

    bool operator>(const PathState& other) const {
        return f() > other.f();
    }
};

// 坐标编码
static inline int encode(const int x, const int y, int gridSize = 1000) {
    return x * gridSize + y;
}

// 获取邻居（四方向）
static std::vector<std::pair<int, int>> getNeighbors(
    const int x, int y,
    const std::function<bool(int, int)>& isWalkable
) {
    std::vector<std::pair<int, int>> result;

    for (int i = 0; i < 4; ++i) {
        constexpr int dx[] = {0, 0, -1, 1};
        constexpr int dy[] = {-1, 1, 0, 0};
        int nx = x + dx[i];
        if (int ny = y + dy[i]; isWalkable(nx, ny)) {
            result.emplace_back(nx, ny);
        }
    }
    return result;
}

// ========== Dijkstra 实现 ==========

PathResult dijkstra(
    const std::function<bool(int, int)>& isWalkable,
    int startX, int startY,
    const int goalX, const int goalY
) {
    PathResult result;
    result.found = false;

    // 起点或终点不可通行
    if (!isWalkable(startX, startY) || !isWalkable(goalX, goalY)) {
        return result;
    }

    if (startX == goalX && startY == goalY) {
        result.found = true;
        result.path.emplace_back(startX, startY);
        result.visitedCount = 1;
        return result;
    }

    // 优先队列：按 g 值排序（Dijkstra 的核心）
    using QueueItem = std::pair<int, std::pair<int, int>>;  // {g, {x, y}}
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> pq;

    // 记录
    std::unordered_map<int, int> dist;        // 到起点的最短距离
    std::unordered_map<int, std::pair<int, int>> parent;  // 前驱节点

    pq.emplace(0, std::make_pair(startX, startY));
    dist[encode(startX, startY)] = 0;
    parent[encode(startX, startY)] = {-1, -1};

    while (!pq.empty()) {
        auto [g, pos] = pq.top();
        pq.pop();
        int cx = pos.first;
        int cy = pos.second;
        ++result.visitedCount;

        // 到达终点
        if (cx == goalX && cy == goalY) {
            result.found = true;
            break;
        }

        // 跳过过时记录
        if (int key = encode(cx, cy); g > dist[key]) continue;

        for (auto [nx, ny] : getNeighbors(cx, cy, isWalkable)) {
            int nKey = encode(nx, ny);
            int newG = g + 1;  // 每步代价为1

            if (auto it = dist.find(nKey); it == dist.end() || newG < it->second) {
                dist[nKey] = newG;
                parent[nKey] = {cx, cy};
                pq.emplace(newG, std::make_pair(nx, ny));
            }
        }
    }

    // 重构路径
    if (result.found) {
        int cx = goalX, cy = goalY;
        while (!(cx == -1 && cy == -1)) {
            result.path.emplace_back(cx, cy);
            int key = encode(cx, cy);
            auto it = parent.find(key);
            if (it == parent.end()) break;
            cx = it->second.first;
            cy = it->second.second;
        }
        std::ranges::reverse(result.path);
    }

    return result;
}

// ========== A* 实现 ==========

PathResult aStar(
    const std::function<bool(int, int)>& isWalkable,
    int startX, int startY,
    const int goalX, const int goalY,
    const HeuristicFunc &heuristic
) {
    PathResult result;
    result.found = false;

    if (!isWalkable(startX, startY) || !isWalkable(goalX, goalY)) {
        return result;
    }

    if (startX == goalX && startY == goalY) {
        result.found = true;
        result.path.emplace_back(startX, startY);
        result.visitedCount = 1;
        return result;
    }

    // 使用自定义结构体替代 tuple
    struct QueueItem {
        int f;
        int g;
        int x;
        int y;

        bool operator>(const QueueItem& other) const {
            return f > other.f;
        }
    };

    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> pq;

    std::unordered_map<int, int> gScore;
    std::unordered_map<int, int> fScore;
    std::unordered_map<int, std::pair<int, int>> parent;

    const int startKey = encode(startX, startY);
    const int startH = heuristic(startX, startY, goalX, goalY);
    gScore[startKey] = 0;
    fScore[startKey] = startH;
    parent[startKey] = std::make_pair(-1, -1);   // ← 用 std::make_pair
    pq.push({startH, 0, startX, startY});

    while (!pq.empty()) {
        const QueueItem item = pq.top();
        pq.pop();
        int cx = item.x;
        int cy = item.y;
        ++result.visitedCount;

        if (cx == goalX && cy == goalY) {
            result.found = true;
            break;
        }

        if (int key = encode(cx, cy); item.g > gScore[key]) continue;

        for (auto [nx, ny] : getNeighbors(cx, cy, isWalkable)) {
            int nKey = encode(nx, ny);
            const int newG = item.g + 1;
            const int newH = heuristic(nx, ny, goalX, goalY);
            const int newF = newG + newH;

            if (auto it = gScore.find(nKey); it == gScore.end() || newG < it->second) {
                gScore[nKey] = newG;
                fScore[nKey] = newF;
                parent[nKey] = std::make_pair(cx, cy);   // ← 用 std::make_pair
                pq.push({newF, newG, nx, ny});
            }
        }
    }

    // 重构路径（不变）
    if (result.found) {
        int cx = goalX, cy = goalY;
        while (!(cx == -1 && cy == -1)) {
            result.path.emplace_back(cx, cy);
            int key = encode(cx, cy);
            auto it = parent.find(key);
            if (it == parent.end()) break;
            cx = it->second.first;
            cy = it->second.second;
        }
        std::ranges::reverse(result.path);
    }

    return result;
}

} // namespace delivery::algo