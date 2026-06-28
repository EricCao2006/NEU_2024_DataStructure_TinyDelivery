#pragma once

#include "Algorithms.h"
#include "../models/Order.h"

#include <vector>
#include <utility>

namespace algo {

    /**
     * @brief 订单合并（稀疏多项式加法）
     * @details 将相同路线的订单视为同类项，合并以减少配送次数
     */
    class OrderMerger : public Algorithm {
    public:
        OrderMerger() = default;
        std::string algorithmName() const override { return "稀疏多项式加法(订单合并)"; }

        /// 多项式中的项：系数=订单权重，指数=(商家ID, 客户ID)
        struct Term {
            int coeff = 0;            // 系数（订单权重/件数）
            std::pair<int, int> exp;  // 指数对 = (merchantId, customerId)
        };

        /**
         * @brief 合并两个有序多项式（稀疏多项式加法）
         */
        std::vector<Term> mergePolynomials(
            const std::vector<Term>& polyA,
            const std::vector<Term>& polyB) const;

        /**
         * @brief 将一批订单按路线分组并合并
         */
        std::vector<Term> mergeOrders(const std::vector<models::Order>& orders) const;
    };

}
