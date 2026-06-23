#include "OrderMerger.h"
#include <algorithm>
#include <cmath>

namespace delivery::algo {

    MergeResult mergeOrders(const std::vector<OrderFeature>& features, const MergeConfig& config) {
        MergeResult result;
        result.originalCount = static_cast<int>(features.size());

        if (features.empty()) {
            return result;
        }

        // 复制并排序（归并思想的核心：有序输入）
        std::vector<OrderFeature> sorted = features;
        std::sort(sorted.begin(), sorted.end());

        // 归并相邻的同特征项
        for (const auto& feat : sorted) {
            if (result.merged.empty()) {
                result.merged.push_back(feat);
                continue;
            }

            auto& last = result.merged.back();

            // 判断是否可以合并
            bool sameKey = (last.merchantId == feat.merchantId &&
                            last.customerId == feat.customerId);
            bool weightOk = (last.weight + feat.weight <= config.maxWeightPerMerge);

            if (sameKey && weightOk) {
                // 合并：累加重量和计数
                last.weight += feat.weight;
                last.count += feat.count;
                result.weightSaved += feat.weight;  // 节省了这部分重量（如果没合并要多跑一趟）
            } else {
                result.merged.push_back(feat);
            }
        }

        result.mergedCount = static_cast<int>(result.merged.size());

        return result;
    }

    std::vector<OrderFeature> fetchFeatures(
        const std::vector<int>& orderIds,
        const std::function<OrderFeature(int)>& getter
    ) {
        std::vector<OrderFeature> result;
        result.reserve(orderIds.size());

        for (int id : orderIds) {
            result.push_back(getter(id));
        }

        return result;
    }

} // namespace delivery::algo