#pragma once

#include "Algorithms.h"
#include "../models/Deliver.h"

#include <vector>

namespace algo {

    /**
     * @brief 骑手选优（堆排序）
     * @details 按综合评分对可用骑手进行排序，选出最优接单人选
     */
    class DeliverSorter : public Algorithm {
    public:
        DeliverSorter() = default;
        std::string algorithmName() const override { return "堆排序(骑手选优)"; }

        /**
         * @brief 堆排序（升序，评分低的排前面 = 更优先）
         */
        void heapSort(std::vector<models::Deliver>& delivers,
                      int merchantX, int merchantY);

        /**
         * @brief 计算骑手综合评分（距离越近、评分越高、负载越少 → 分越低越优先）
         */
        static double calcScore(const models::Deliver& d,
                                int merchantX, int merchantY);

    private:
        void heapify(std::vector<models::Deliver>& arr, int n, int i,
                     int merchantX, int merchantY);
    };

}
