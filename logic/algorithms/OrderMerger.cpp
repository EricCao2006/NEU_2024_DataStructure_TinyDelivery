#include "OrderMerger.h"

namespace algo {

    std::vector<OrderMerger::Term> OrderMerger::mergePolynomials(
        const std::vector<Term>& polyA,
        const std::vector<Term>& polyB) const {
        callCount++;
        std::vector<Term> result;
        int i = 0, j = 0;

        // 双指针归并（类似稀疏多项式加法）
        while (i < static_cast<int>(polyA.size()) && j < static_cast<int>(polyB.size())) {
            auto& expA = polyA[i].exp;

            if (auto& expB = polyB[j].exp; expA < expB) {
                result.push_back(polyA[i++]);
            } else if (expA > expB) {
                result.push_back(polyB[j++]);
            } else {
                // 指数相同 → 系数相加（合并同类项）
                if (const int sumCoeff = polyA[i].coeff + polyB[j].coeff; sumCoeff != 0) {
                    result.push_back({sumCoeff, expA});
                }
                i++;
                j++;
            }
            count++;
        }

        // 处理剩余项
        while (i < static_cast<int>(polyA.size())) result.push_back(polyA[i++]);
        while (j < static_cast<int>(polyB.size())) result.push_back(polyB[j++]);

        return result;
    }

    std::vector<OrderMerger::Term> OrderMerger::mergeOrders(
        const std::vector<models::Order>& orders) const {
        callCount++;
        // 按 (merchantId, customerId) 分组统计
        std::map<std::pair<int, int>, int> groups;
        for (const auto& order : orders) {
            auto key = std::make_pair(order.getMerchantId(), order.getCustomerId());
            groups[key]++;
            count++;
        }

        // 转为有序多项式（map 自动按 key 排序）
        std::vector<Term> result;
        for (const auto& [exp, coeff] : groups) {
            result.push_back({coeff, exp});
        }
        return result;
    }

}
