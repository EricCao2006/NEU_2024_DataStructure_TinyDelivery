#include "DeliverSorter.h"
#include <algorithm>
#include <climits>

namespace delivery::algo {

    int computeScore(const DeliverScore& score, const SortWeights& weights) {
        // 距离归一化（假设最大距离100）
        const int distScore = std::min(100, score.distance);
        // 经验归一化（假设最大经验1000）
        const int expScore = std::min(100, score.experience / 10);
        // 负载归一化（假设最大负载10）
        const int loadScore = std::min(100, score.currentLoad * 10);

        // 得分 = 距离权重×距离 + 经验权重×(100-经验) + 负载权重×负载
        // 经验越高得分越低（因为经验维度是反向的）
        return weights.distanceWeight * distScore
             + weights.experienceWeight * (100 - expScore)
             + weights.loadWeight * loadScore;
    }

    SortResult rankDelivers(const std::vector<DeliverScore>& scores, const SortWeights& weights, int topK) {
        SortResult result;
        result.totalCandidates = static_cast<int>(scores.size());

        if (scores.empty()) {
            return result;
        }

        // 复制并计算综合得分
        std::vector<std::pair<int, int>> ranked;  // {score, id}
        ranked.reserve(scores.size());

        for (const auto& s : scores) {
            int score = computeScore(s, weights);
            ranked.emplace_back(score, s.id);
        }

        // 按得分排序（得分越低越优）
        std::ranges::sort(ranked,
                          [](const auto& a, const auto& b) {
                              return a.first < b.first;
                          }
        );

        result.considered = static_cast<int>(ranked.size());

        // 取前K个
        const int count = topK > 0 ? std::min(topK, static_cast<int>(ranked.size())) : static_cast<int>(ranked.size());
        result.rankedIds.reserve(count);
        for (int i = 0; i < count; ++i) {
            result.rankedIds.push_back(ranked[i].second);
        }

        return result;
    }

} // namespace delivery::algo