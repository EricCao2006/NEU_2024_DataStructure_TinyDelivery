#pragma once

#include "Algorithms.h"

namespace algo {
    /**
     * @brief 订单合并（稀疏多项式加法）
     * @todo 订单合并（稀疏多项式加法）
     */
    class OrderMerger : public algo::Algorithm {
    public:
        OrderMerger() = default;
        ~OrderMerger() = default;

        void merge();
    };
}