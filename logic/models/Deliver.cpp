#include "Deliver.h"
#include "../utils/math_.h"

namespace models {

    Deliver::Deliver(const int id, const QString& name)
        : m_id(id), m_name(name) {}

    void Deliver::removeOrderId(const int orderId) {
        if (const auto it = std::ranges::find(m_orderIds, orderId); it != m_orderIds.end()) {
            m_orderIds.erase(it);
        }
    }

    bool Deliver::moveToward(const int targetX, const int targetY) {
        if (m_x == targetX && m_y == targetY) {
            return true;
        }

        const int dx = targetX - m_x;
        const int dy = targetY - m_y;

        // 沿距离较大的方向优先移动（曼哈顿风格）
        if (std::abs(dx) >= std::abs(dy)) {
            const int step = utils::clamp(dx, -m_speed, m_speed);
            m_x += step;
        } else {
            const int step = utils::clamp(dy, -m_speed, m_speed);
            m_y += step;
        }

        return (m_x == targetX && m_y == targetY);
    }

    utils::json Deliver::toJsonObject() const {
        return {
            {"id", m_id},
            {"name", m_name.toStdString()},
            {"x", m_x},
            {"y", m_y},
            {"speed", m_speed},
            {"max_capacity", m_maxCapacity},
            {"current_load", m_currentLoad},
            {"total_earnings", m_totalEarnings},
            {"completed_orders", m_completedOrders},
            {"rating", m_rating},
            {"order_ids", m_orderIds}
        };
    }

    Deliver Deliver::fromJson(const utils::json& j) {
        Deliver d;
        d.m_id = j.value("id", 0);
        d.m_name = QString::fromStdString(j.value("name", ""));
        d.m_x = j.value("x", 0);
        d.m_y = j.value("y", 0);
        d.m_speed = j.value("speed", 2);
        d.m_maxCapacity = j.value("max_capacity", 30);
        d.m_currentLoad = j.value("current_load", 0);
        d.m_totalEarnings = j.value("total_earnings", 0.0);
        d.m_completedOrders = j.value("completed_orders", 0);
        d.m_rating = j.value("rating", 5.0);
        if (j.contains("order_ids") && j["order_ids"].is_array()) {
            d.m_orderIds = j["order_ids"].get<std::vector<int>>();
        }
        return d;
    }

    // ── Vehicle ──

    utils::json Vehicle::toJsonObject() const {
        return {
            {"id", id},
            {"name", name.toStdString()},
            {"speed", speed},
            {"capacity", capacity},
            {"cost_per_km", costPerKm}
        };
    }

    Vehicle Vehicle::fromJson(const utils::json& j) {
        Vehicle v;
        v.id = j.value("id", 0);
        v.name = QString::fromStdString(j.value("name", ""));
        v.speed = j.value("speed", 2);
        v.capacity = j.value("capacity", 30);
        v.costPerKm = j.value("cost_per_km", 0.3);
        return v;
    }

}
