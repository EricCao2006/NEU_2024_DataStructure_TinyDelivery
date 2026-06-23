#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace delivery {

/**
 * @brief 订单状态枚举
 */
enum class OrderStatus : uint8_t {
    PENDING,      ///< 待分配
    ASSIGNED,     ///< 已分配骑手
    PICKING,      ///< 取货中
    DELIVERING,   ///< 派送中
    COMPLETED,    ///< 已完成
    CANCELLED     ///< 已取消
};

/**
 * @brief 订单类，表示从商家到客户的一次配送请求
 */
class Order {
public:
    Order() = default;
    Order(int id, int merchantId, int customerId, double weight);

    // ========== Getter ==========
    int id() const { return m_id; }
    int merchantId() const { return m_merchantId; }
    int customerId() const { return m_customerId; }
    double weight() const { return m_weight; }
    OrderStatus status() const { return m_status; }
    int deliverId() const { return m_deliverId; }
    const std::vector<std::pair<int, int>>& route() const { return m_route; }
    int currentStep() const { return m_currentStep; }
    int createTime() const { return m_createTime; }          ///< 游戏内时间戳
    int priority() const { return m_priority; }              ///< 优先级（VIP客户更高）

    // ========== Setter ==========
    void setStatus(OrderStatus status) { m_status = status; }
    void setDeliverId(int id) { m_deliverId = id; }
    void setRoute(const std::vector<std::pair<int, int>>& route) { m_route = route; }
    void setPriority(int priority) { m_priority = priority; }

    /**
     * @brief 沿路径前进一格，返回是否到达终点
     * @return true 表示到达终点（最后一个节点）
     */
    bool advance();

    /**
     * @brief 获取当前坐标
     * @return (x, y) 坐标对，若路径为空则返回 (-1, -1)
     */
    std::pair<int, int> currentPosition() const;

    /**
     * @brief 检查订单是否已到达最终目的地
     */
    bool isCompleted() const { return m_status == OrderStatus::COMPLETED; }

private:
    int m_id = -1;                                               ///< 订单唯一ID
    int m_merchantId = -1;                                       ///< 发件商家ID
    int m_customerId = -1;                                       ///< 收件客户ID
    double m_weight = 0.0;                                       ///< 包裹重量（kg）
    OrderStatus m_status = OrderStatus::PENDING;                ///< 当前状态
    int m_deliverId = -1;                                        ///< 分配的骑手ID（-1表示未分配）
    std::vector<std::pair<int, int>> m_route;                   ///< 完整路径坐标序列
    int m_currentStep = 0;                                       ///< 当前所在路径索引
    int m_createTime = 0;                                        ///< 创建时的游戏时间
    int m_priority = 0;                                          ///< 0=普通, 1=VIP
};

} // namespace delivery