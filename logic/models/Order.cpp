#include "Order.h"
#include "../utils/math_.h"

namespace models {

    Order::Order(const int id, const int merchantId, const int customerId, const int distance)
        : m_id(id), m_merchantId(merchantId), m_customerId(customerId), m_distance(distance) {
        calculateFee();
    }

    void Order::calculateFee() {
        // 基础费 = 距离 × 1.5元
        const double baseFee = m_distance * 1.5;
        // VIP加成：每级 +15%
        const double vipMultiplier = 1.0 + m_vipLevel * 0.15;
        m_deliveryFee = baseFee * vipMultiplier;
    }

    utils::json Order::toJsonObject() const {
        return {
            {"id", m_id},
            {"merchant_id", m_merchantId},
            {"customer_id", m_customerId},
            {"status", static_cast<int>(m_status)},
            {"distance", m_distance},
            {"delivery_fee", m_deliveryFee},
            {"vip_level", m_vipLevel},
            {"create_time", m_createTime},
            {"assign_time", m_assignTime},
            {"complete_time", m_completeTime}
        };
    }

    Order Order::fromJson(const utils::json& j) {
        Order o;
        o.m_id = j.value("id", 0);
        o.m_merchantId = j.value("merchant_id", 0);
        o.m_customerId = j.value("customer_id", 0);
        o.m_status = static_cast<OrderStatus>(j.value("status", 0));
        o.m_distance = j.value("distance", 0);
        o.m_deliveryFee = j.value("delivery_fee", 0.0);
        o.m_vipLevel = j.value("vip_level", 0);
        o.m_createTime = j.value("create_time", 0);
        o.m_assignTime = j.value("assign_time", 0);
        o.m_completeTime = j.value("complete_time", 0);
        return o;
    }

}
