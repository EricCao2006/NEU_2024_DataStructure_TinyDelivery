#pragma once

#include "Enums.h"
#include "../utils/json_.h"

#include <QString>

namespace models {

    class Order;

    /**
     * @brief 商家
     */
    class Merchant {
    public:
        Merchant() = default;
        Merchant(int id, const QString& name, int x, int y, MerchantType type);

        [[nodiscard]] int getId() const { return m_id; }
        [[nodiscard]] const QString& getName() const { return m_name; }
        [[nodiscard]] int getX() const { return m_x; }
        [[nodiscard]] int getY() const { return m_y; }
        [[nodiscard]] MerchantType getType() const { return m_type; }
        [[nodiscard]] int getTotalOrders() const { return m_totalOrders; }
        [[nodiscard]] double getTotalRevenue() const { return m_totalRevenue; }

        void setTotalOrders(int n) { m_totalOrders = n; }
        void setTotalRevenue(double r) { m_totalRevenue = r; }
        void addOrder(double fee) { m_totalOrders++; m_totalRevenue += fee; }

        [[nodiscard]] utils::json toJsonObject() const;
        static Merchant fromJson(const utils::json& j);

    private:
        int m_id = 0;
        QString m_name;
        int m_x = 0;
        int m_y = 0;
        MerchantType m_type = MerchantType::Food;
        int m_totalOrders = 0;
        double m_totalRevenue = 0.0;
    };

}
