#pragma once
/**
 * @file OrderMerger.h
 * @brief 订单合并算法
 * @details 基于稀疏多项式加法的归并思想
 *          将同路线/同目的地的订单合并，减少空驶
 */

#include <vector>
#include <utility>
#include <functional>

namespace delivery::algo {

    /**
     * @brief 订单特征（类似多项式的一项）
     */
    struct OrderFeature {
        int merchantId;           ///< 商家ID（相当于变量）
        int customerId;           ///< 客户ID
        double weight;            ///< 总重量（相当于系数）
        int count;                ///< 合并的订单数

        /**
         * @brief 比较函数，用于排序（按商家ID -> 客户ID）
         */
        bool operator<(const OrderFeature& other) const {
            if (merchantId != other.merchantId) return merchantId < other.merchantId;
            return customerId < other.customerId;
        }

        bool operator==(const OrderFeature& other) const {
            return merchantId == other.merchantId && customerId == other.customerId;
        }
    };

    /**
     * @brief 合并配置
     */
    struct MergeConfig {
        double maxWeightPerMerge = 50.0;   ///< 单次合并最大重量（kg）
        bool requireSameMerchant = true;   ///< 是否要求同一商家
    };

    /**
     * @brief 订单合并结果
     */
    struct MergeResult {
        std::vector<OrderFeature> merged;  ///< 合并后的订单组
        int originalCount = 0;             ///< 原始订单数
        int mergedCount = 0;               ///< 合并后组数
        double weightSaved = 0.0;          ///< 节省的运力（重量差）
    };

    /**
     * @brief 合并订单
     * @details 输入一组订单特征，输出合并后的特征列表
     *          基于归并思想：先排序再合并相邻的同特征项
     * @param features 输入订单特征列表
     * @param config 合并配置
     * @return MergeResult
     */
    MergeResult mergeOrders(
        const std::vector<OrderFeature>& features,
        const MergeConfig& config = MergeConfig{}
    );

    /**
     * @brief 从订单ID列表获取特征
     * @param orderIds 订单ID列表
     * @param getter 获取订单特征的函数: OrderFeature(int orderId)
     * @return 特征列表
     */
    std::vector<OrderFeature> fetchFeatures(
        const std::vector<int>& orderIds,
        const std::function<OrderFeature(int)>& getter
    );

} // namespace delivery::algo