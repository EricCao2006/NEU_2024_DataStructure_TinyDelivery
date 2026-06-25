#include "DispatchSystem.h"
#include "../algorithms/Algorithms.h"
#include "../utils/log_.h"
#include "../utils/math_.h"
#include "../GameState.h"

namespace delivery {

// ========== 处理所有待分配订单 ==========
void DispatchSystem::processOrders() {
    auto& state = GameState::instance();

    if (state.pendingOrderIds.empty()) {
        return;
    }

    // 按优先级排序：VIP优先
    std::ranges::sort(state.pendingOrderIds,
                      [&state](const int a, const int b) {
                          const auto* orderA = state.findOrder(a);
                          const auto* orderB = state.findOrder(b);
                          if (!orderA || !orderB) return false;
                          return orderA->priority() > orderB->priority();
                      });

    // 逐个处理
    auto it = state.pendingOrderIds.begin();
    while (it != state.pendingOrderIds.end()) {
        if (const int orderId = *it; processOrder(orderId)) {
            // 分配成功，从待分配队列移除
            it = state.pendingOrderIds.erase(it);
        } else {
            ++it;
        }
    }
}

// ========== 处理单个订单 ==========
bool DispatchSystem::processOrder(const int orderId) {
    auto& state = GameState::instance();
    auto* order = state.findOrder(orderId);

    if (!order) {
        LOG_WARN("订单 #" + std::to_string(orderId) + " 不存在");
        return false;
    }

    if (order->status() != OrderStatus::PENDING) {
        return false;
    }

    // 1. 计算路径
    if (!calculatePath(orderId)) {
        LOG_DEBUG("订单 #" + std::to_string(orderId) + " 路径计算失败（可能无通路）");
        return false;
    }

    // 2. 分配骑手
    const int deliverId = assignDeliver(orderId);
    if (deliverId < 0) {
        LOG_DEBUG("订单 #" + std::to_string(orderId) + " 无可用骑手");
        return false;
    }

    // 3. 更新状态
    order->setStatus(OrderStatus::ASSIGNED);
    order->setDeliverId(deliverId);

    auto* deliver = state.findDeliver(deliverId);
    if (deliver) {
        deliver->setStatus(DeliverStatus::PICKING);
        deliver->setCurrentOrderId(orderId);
    }

    LOG_INFO("订单 #" + std::to_string(orderId) +
             " 分配给骑手 #" + std::to_string(deliverId));
    return true;
}

// ========== 分配骑手 ==========
int DispatchSystem::assignDeliver(const int orderId) {
    auto& state = GameState::instance();
    const auto* order = state.findOrder(orderId);

    if (!order) return -1;

    // 获取订单起点（商家位置）
    const auto* merchant = state.findMerchant(order->merchantId());
    if (!merchant) return -1;

    // 获取空闲骑手
    const auto idleIds = state.getIdleDelivers();
    if (idleIds.empty()) return -1;

    // 构建骑手评分列表
    std::vector<algo::DeliverScore> scores;
    scores.reserve(idleIds.size());

    for (const int did : idleIds) {
        const auto* deliver = state.findDeliver(did);
        if (!deliver) continue;

        // 计算到商家的距离
        const int dist = deliver->distanceTo(merchant->x(), merchant->y());

        // 计算当前负载（已分配订单数）
        int load = 0;
        for (const auto& o : state.orders) {
            if (o.deliverId() == did && o.status() != OrderStatus::COMPLETED) {
                ++load;
            }
        }

        algo::DeliverScore score{};
        score.id = did;
        score.distance = dist;
        score.experience = deliver->experience();
        score.currentLoad = load;
        scores.push_back(score);
    }

    if (scores.empty()) return -1;

    // 使用排序算法选出最优骑手
    algo::SortWeights weights;
    weights.distanceWeight = 5;
    weights.experienceWeight = 3;
    weights.loadWeight = 4;

    const auto result = algo::rankDelivers(scores, weights, 1);
    if (result.rankedIds.empty()) return -1;

    return result.rankedIds[0];
}

// ========== 计算路径 ==========
bool DispatchSystem::calculatePath(const int orderId) {
    auto& state = GameState::instance();
    auto* order = state.findOrder(orderId);

    if (!order) return false;

    const auto* merchant = state.findMerchant(order->merchantId());
    const auto* customer = state.findCustomer(order->customerId());

    if (!merchant || !customer) return false;

    // 设置可通行检查函数（使用城市网格）
    auto isWalkable = [&state](const int x, const int y) -> bool {
        return state.cityGrid.isWalkable(x, y);
    };

    // 使用 A* 算法计算路径
    auto result = algo::aStar(
        isWalkable,
        merchant->x(), merchant->y(),
        customer->x(), customer->y()
    );

    if (!result.found) {
        // 降级使用 Dijkstra
        result = algo::dijkstra(
            isWalkable,
            merchant->x(), merchant->y(),
            customer->x(), customer->y()
        );
    }

    if (!result.found) {
        LOG_WARN("订单 #" + std::to_string(orderId) + " 无路径");
        return false;
    }

    // 设置路径
    order->setRoute(result.path);
    LOG_DEBUG("订单 #" + std::to_string(orderId) +
              " 路径计算完成，步数: " + std::to_string(result.path.size()) +
              "，访问节点: " + std::to_string(result.visitedCount));

    return true;
}

} // namespace delivery