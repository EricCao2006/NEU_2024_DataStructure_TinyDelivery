#pragma once

#include "Algorithms.h"

namespace algo {
    /**
     * @brief 骑手选优（多维度排序算法）
     * @todo 骑手选优（多维度排序算法）
     */
    class DeliverSorter : public algo::Algorithm {
    public:
        DeliverSorter() = default;
        ~DeliverSorter() = default;

        void sort();
    };
}