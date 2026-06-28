#pragma once

#include "../utils/json_.h"

#include <QString>

namespace models {

    /**
     * @brief 客户
     */
    class Customer {
    public:
        Customer() = default;
        Customer(int id, const QString& name, int x, int y, int vipLevel);

        [[nodiscard]] int getId() const { return m_id; }
        [[nodiscard]] const QString& getName() const { return m_name; }
        [[nodiscard]] int getX() const { return m_x; }
        [[nodiscard]] int getY() const { return m_y; }
        [[nodiscard]] int getVipLevel() const { return m_vipLevel; }
        [[nodiscard]] int getTotalOrders() const { return m_totalOrders; }
        [[nodiscard]] double getSatisfaction() const { return m_satisfaction; }

        void setTotalOrders(int n) { m_totalOrders = n; }
        void setSatisfaction(double s) { m_satisfaction = s; }
        void addOrder() { m_totalOrders++; }

        /**
         * @brief 根据VIP等级计算满意度加成
         */
        [[nodiscard]] double calculateHappiness() const;

        [[nodiscard]] utils::json toJsonObject() const;
        static Customer fromJson(const utils::json& j);

    private:
        int m_id = 0;
        QString m_name;
        int m_x = 0;
        int m_y = 0;
        int m_vipLevel = 0;
        int m_totalOrders = 0;
        double m_satisfaction = 100.0;
    };

}
