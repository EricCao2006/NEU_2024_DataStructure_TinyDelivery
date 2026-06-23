#pragma once
/**
 * @file DeliverSorter.h
 * @brief 骑手多维度排序算法
 * @details 基于基数排序思想，按多个维度（距离、经验、负载）对骑手排序
 *          用于从候选骑手中选出最优派送员
 */

#include <vector>
#include <functional>
#include <cstdint>

namespace delivery::algo {

    /**
     * @brief 骑手评分维度枚举
     */
    enum class SortDimension : uint8_t {
        DISTANCE,      ///< 距离（越小越好）
        EXPERIENCE,    ///< 经验（越大越好）
        LOAD           ///< 当前负载（越小越好）
    };

    /**
     * @brief 骑手排序记录
     */
    struct DeliverScore {
        int id;                    ///< 骑手ID
        int distance;              ///< 到取货点的距离
        int experience;            ///< 经验值
        int currentLoad;           ///< 当前负载
        int score;                 ///< 综合得分（越小越优）
    };

    /**
     * @brief 排序结果
     */
    struct SortResult {
        std::vector<int> rankedIds;   ///< 排序后的骑手ID列表（最优在前）
        int totalCandidates = 0;      ///< 候选总数
        int considered = 0;           ///< 实际参与排序的数量
    };

    /**
     * @brief 权重配置
     */
    struct SortWeights {
        int distanceWeight = 5;      ///< 距离权重
        int experienceWeight = 3;    ///< 经验权重（越高越需要经验丰富）
        int loadWeight = 4;          ///< 负载权重
    };

    /**
     * @brief 多维度骑手排序
     * @details 使用基数排序的多轮思想：按距离→负载→经验逐轮筛选
     *          每一轮只关心当前维度，类似基数排序按位处理
     * @param scores 骑手评分列表
     * @param weights 各维度权重
     * @param topK 只返回前K个（0表示全部）
     * @return SortResult
     */
    SortResult rankDelivers(
        const std::vector<DeliverScore>& scores,
        const SortWeights& weights = SortWeights{},
        int topK = 0
    );

    /**
     * @brief 计算综合得分
     * @details 归一化后加权求和，得分越低越优
     */
    int computeScore(const DeliverScore& score, const SortWeights& weights);

} // namespace delivery::algo