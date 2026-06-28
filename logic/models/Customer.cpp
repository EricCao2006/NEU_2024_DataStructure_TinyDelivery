#include "Customer.h"

namespace models {

    Customer::Customer(const int id, const QString& name, const int x, const int y, const int vipLevel)
        : m_id(id), m_name(name), m_x(x), m_y(y), m_vipLevel(vipLevel) {}

    double Customer::calculateHappiness() const {
        // VIP等级越高，基础满意度越高
        // 基础满意度 = 80 + VIP等级 × 5
        const double base = 80.0 + m_vipLevel * 5.0;
        // 与当前满意度取平均
        return (base + m_satisfaction) / 2.0;
    }

    utils::json Customer::toJsonObject() const {
        return {
            {"id", m_id},
            {"name", m_name.toStdString()},
            {"x", m_x},
            {"y", m_y},
            {"vip_level", m_vipLevel},
            {"total_orders", m_totalOrders},
            {"satisfaction", m_satisfaction}
        };
    }

    Customer Customer::fromJson(const utils::json& j) {
        Customer c;
        c.m_id = j.value("id", 0);
        c.m_name = QString::fromStdString(j.value("name", ""));
        c.m_x = j.value("x", 0);
        c.m_y = j.value("y", 0);
        c.m_vipLevel = j.value("vip_level", 0);
        c.m_totalOrders = j.value("total_orders", 0);
        c.m_satisfaction = j.value("satisfaction", 100.0);
        return c;
    }

}
