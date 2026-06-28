#include "PathFinder.h"

namespace algo {

    std::vector<std::pair<int, int>> PathFinder::findPathDijkstra(
        const models::CityGrid& grid,
        const int startX, const int startY, const int endX, const int endY)
    {
        callCount++;
        const int size = grid.getSize();
        std::vector<std::vector<int>> dist(size, std::vector<int>(size, INT32_MAX));
        std::vector<std::vector<int>> parentX(size, std::vector<int>(size, -1));
        std::vector<std::vector<int>> parentY(size, std::vector<int>(size, -1));
        std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

        std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;

        dist[startY][startX] = 0;
        pq.push({startX, startY, 0, 0});

        while (!pq.empty()) {
            const Node cur = pq.top(); pq.pop();
            count++;

            if (cur.x == endX && cur.y == endY) {
                return reconstructPath(parentX, parentY, startX, startY, endX, endY);
            }

            if (visited[cur.y][cur.x]) continue;
            visited[cur.y][cur.x] = true;

            for (int d = 0; d < 4; d++) {
                const int nx = cur.x + DX[d];
                const int ny = cur.y + DY[d];

                if (grid.isWalkable(nx, ny) && !visited[ny][nx]) {
                    const int newDist = dist[cur.y][cur.x] + 1;
                    if (newDist < dist[ny][nx]) {
                        dist[ny][nx] = newDist;
                        parentX[ny][nx] = cur.x;
                        parentY[ny][nx] = cur.y;
                        pq.push({nx, ny, newDist, newDist});
                    }
                }
            }
        }
        return {};  // 无路径
    }

    std::vector<std::pair<int, int>> PathFinder::findPathAStar(
        const models::CityGrid& grid,
        const int startX, const int startY, const int endX, const int endY)
    {
        callCount++;
        const int size = grid.getSize();
        std::vector<std::vector<int>> gCost(size, std::vector<int>(size, INT32_MAX));
        std::vector<std::vector<int>> parentX(size, std::vector<int>(size, -1));
        std::vector<std::vector<int>> parentY(size, std::vector<int>(size, -1));
        std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

        std::priority_queue<Node, std::vector<Node>, std::greater<>> pq;

        gCost[startY][startX] = 0;
        const int h = std::abs(endX - startX) + std::abs(endY - startY);  // 曼哈顿距离
        pq.push({startX, startY, 0, h});

        while (!pq.empty()) {
            const Node cur = pq.top(); pq.pop();
            count++;

            if (cur.x == endX && cur.y == endY) {
                return reconstructPath(parentX, parentY, startX, startY, endX, endY);
            }

            if (visited[cur.y][cur.x]) continue;
            visited[cur.y][cur.x] = true;

            for (int d = 0; d < 4; d++) {
                const int nx = cur.x + DX[d];
                const int ny = cur.y + DY[d];

                if (grid.isWalkable(nx, ny) && !visited[ny][nx]) {
                    const int newG = gCost[cur.y][cur.x] + 1;
                    if (newG < gCost[ny][nx]) {
                        gCost[ny][nx] = newG;
                        const int newH = std::abs(endX - nx) + std::abs(endY - ny);
                        parentX[ny][nx] = cur.x;
                        parentY[ny][nx] = cur.y;
                        pq.push({nx, ny, newG, newG + newH});
                    }
                }
            }
        }
        return {};
    }

    std::vector<std::pair<int, int>> PathFinder::reconstructPath(
        const std::vector<std::vector<int>>& parentX,
        const std::vector<std::vector<int>>& parentY,
        int startX, int startY, const int endX, const int endY)
    {
        std::vector<std::pair<int, int>> path;
        int cx = endX, cy = endY;
        while (cx != startX || cy != startY) {
            path.emplace_back(cx, cy);
            const int px = parentX[cy][cx];
            const int py = parentY[cy][cx];
            cx = px;
            cy = py;
        }
        path.emplace_back(startX, startY);
        std::ranges::reverse(path);
        return path;
    }

}
