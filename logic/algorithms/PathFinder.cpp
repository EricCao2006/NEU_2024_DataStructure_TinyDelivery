#include "PathFinder.h"
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <cmath>

namespace delivery::algo {

// ========== 辅助结构 ==========

struct PathState {
    int x, y;
    int g;
    int h;
    int parentX, parentY;

    int f() const { return g + h; }

    bool operator>(const PathState& other) const {
        return f() > other.f();
    }
};

// 坐标编码
static inline int encode(int x, int y, int gridSize = 1000) {
    return x * gridSize + y;
}

// 获取邻居（四方向）
static std::vector<std::pair<int, int>> getNeighbors(
    int x, int y,
    const std::function<bool(int, int)>& isWalkable
) {
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

// ========== Dijkstra 实现 ==========

PathResult dijkstra(
    const std::function<bool(int, int)>& isWalkable,
    int startX, int startY,
    int goalX, int goalY
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
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> pq;

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
        int key = encode(cx, cy);
        if (g > dist[key]) continue;

        for (auto [nx, ny] : getNeighbors(cx, cy, isWalkable)) {
            int nKey = encode(nx, ny);
            int newG = g + 1;  // 每步代价为1

            auto it = dist.find(nKey);
            if (it == dist.end() || newG < it->second) {
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
        std::reverse(result.path.begin(), result.path.end());
    }

    return result;
}

// ========== A* 实现 ==========

PathResult aStar(
    const std::function<bool(int, int)>& isWalkable,
    int startX, int startY,
    int goalX, int goalY,
    HeuristicFunc heuristic
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

    // 优先队列：按 f = g + h 排序（A* 的核心）
    using QueueItem = std::tuple<int, int, int>;  // {f, g, x, y}
    auto cmp = [](const QueueItem& a, const QueueItem& b) {
        return std::get<0>(a) > std::get<0>(b);
    };
    std::priority_queue<QueueItem, std::vector<QueueItem>, decltype(cmp)> pq(cmp);

    std::unordered_map<int, int> gScore;          // 到起点的实际代价
    std::unordered_map<int, int> fScore;          // 估计总代价
    std::unordered_map<int, std::pair<int, int>> parent;

    int startKey = encode(startX, startY);
    int startH = heuristic(startX, startY, goalX, goalY);
    gScore[startKey] = 0;
    fScore[startKey] = startH;
    pq.emplace(startH, 0, startX, startY);
    parent[startKey] = {-1, -1};

    while (!pq.empty()) {
        auto [f, g, cx, cy] = pq.top();
        pq.pop();
        ++result.visitedCount;

        if (cx == goalX && cy == goalY) {
            result.found = true;
            break;
        }

        int key = encode(cx, cy);
        if (g > gScore[key]) continue;

        for (auto [nx, ny] : getNeighbors(cx, cy, isWalkable)) {
            int nKey = encode(nx, ny);
            int newG = g + 1;
            int newH = heuristic(nx, ny, goalX, goalY);
            int newF = newG + newH;

            auto it = gScore.find(nKey);
            if (it == gScore.end() || newG < it->second) {
                gScore[nKey] = newG;
                fScore[nKey] = newF;
                parent[nKey] = {cx, cy};
                pq.emplace(newF, newG, nx, ny);
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
        std::reverse(result.path.begin(), result.path.end());
    }

    return result;
}

} // namespace delivery::algo