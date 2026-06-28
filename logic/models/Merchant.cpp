#include "Merchant.h"

namespace models {

    Merchant::Merchant(const int id, const QString& name, const int x, const int y, const MerchantType type)
        : m_id(id), m_name(name), m_x(x), m_y(y), m_type(type) {}

    utils::json Merchant::toJsonObject() const {
        return {
            {"id", m_id},
            {"name", m_name.toStdString()},
            {"x", m_x},
            {"y", m_y},
            {"type", static_cast<int>(m_type)},
            {"total_orders", m_totalOrders},
            {"total_revenue", m_totalRevenue}
        };
    }

    Merchant Merchant::fromJson(const utils::json& j) {
        Merchant m;
        m.m_id = j.value("id", 0);
        m.m_name = QString::fromStdString(j.value("name", ""));
        m.m_x = j.value("x", 0);
        m.m_y = j.value("y", 0);
        m.m_type = static_cast<MerchantType>(j.value("type", 0));
        m.m_totalOrders = j.value("total_orders", 0);
        m.m_totalRevenue = j.value("total_revenue", 0.0);
        return m;
    }

}
