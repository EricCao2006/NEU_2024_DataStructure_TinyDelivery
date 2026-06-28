#include "DeliverSorter.h"
#include "../utils/math_.h"

namespace algo {

    double DeliverSorter::calcScore(const models::Deliver& d, int merchantX, int merchantY) {
        // 距离分：曼哈顿距离，越近越好
        const int dist = utils::manhattanDistance(d.getX(), d.getY(), merchantX, merchantY);
        const double distScore = dist * 10.0;

        // 评分分：骑手评分越高越好（反转：10 - rating）
        const double ratingScore = (10.0 - d.getRating()) * 5.0;

        // 负载分：剩余载量越大越好
        const int remainCapacity = d.getMaxCapacity() - d.getCurrentLoad();
        const double capacityScore = (30 - remainCapacity) * 2.0;

        return distScore + ratingScore + capacityScore;
    }

    void DeliverSorter::heapify(std::vector<models::Deliver>& arr, int n, int i,
                                 const int merchantX, const int merchantY)
    {
        int largest = i;
        const int left = 2 * i + 1;
        const int right = 2 * i + 2;

        if (left < n && calcScore(arr[left], merchantX, merchantY) > calcScore(arr[largest], merchantX, merchantY)) {
            largest = left;
        }
        if (right < n && calcScore(arr[right], merchantX, merchantY) > calcScore(arr[largest], merchantX, merchantY)) {
            largest = right;
        }

        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            count++;
            heapify(arr, n, largest, merchantX, merchantY);
        }
    }

    void DeliverSorter::heapSort(std::vector<models::Deliver>& delivers,
                                  const int merchantX, const int merchantY)
    {
        callCount++;
        const int n = static_cast<int>(delivers.size());
        if (n <= 1) return;

        // 建大顶堆
        for (int i = n / 2 - 1; i >= 0; i--) {
            heapify(delivers, n, i, merchantX, merchantY);
        }

        // 逐个取出堆顶（最大值）放到末尾
        for (int i = n - 1; i > 0; i--) {
            std::swap(delivers[0], delivers[i]);
            count++;
            heapify(delivers, i, 0, merchantX, merchantY);
        }
        // 排序后：评分低的在前（更优先），评分高的在后
    }

}
