#pragma once

#include "../utils/json_.h"

#include <QString>
#include <vector>
#include <algorithm>

namespace models {

    /**
     * @brief 骑手
     */
    class Deliver {
    public:
        Deliver() = default;
        Deliver(int id, const QString& name);

        [[nodiscard]] int getId() const { return m_id; }
        [[nodiscard]] const QString& getName() const { return m_name; }
        [[nodiscard]] int getX() const { return m_x; }
        [[nodiscard]] int getY() const { return m_y; }
        [[nodiscard]] int getSpeed() const { return m_speed; }
        [[nodiscard]] int getMaxCapacity() const { return m_maxCapacity; }
        [[nodiscard]] int getCurrentLoad() const { return m_currentLoad; }
        [[nodiscard]] double getTotalEarnings() const { return m_totalEarnings; }
        [[nodiscard]] int getCompletedOrders() const { return m_completedOrders; }
        [[nodiscard]] double getRating() const { return m_rating; }

        void setPosition(int x, int y) { m_x = x; m_y = y; }
        void setSpeed(int s) { m_speed = s; }
        void setMaxCapacity(int c) { m_maxCapacity = c; }
        void addLoad(int amount) { m_currentLoad += amount; }
        void removeLoad(int amount) { m_currentLoad = std::max(0, m_currentLoad - amount); }
        void addEarnings(double amount) { m_totalEarnings += amount; }
        void incrementCompleted() { m_completedOrders++; }
        void setRating(double r) { m_rating = r; }

        void addOrderId(int orderId) { m_orderIds.push_back(orderId); }
        void removeOrderId(int orderId);
        [[nodiscard]] const std::vector<int>& getOrderIds() const { return m_orderIds; }

        /**
         * @brief 向目标坐标移动一步，返回是否到达
         */
        bool moveToward(int targetX, int targetY);

        [[nodiscard]] utils::json toJsonObject() const;
        static Deliver fromJson(const utils::json& j);

    private:
        int m_id = 0;
        QString m_name;
        int m_x = 0;
        int m_y = 0;
        int m_speed = 2;
        int m_maxCapacity = 30;
        int m_currentLoad = 0;
        double m_totalEarnings = 0.0;
        int m_completedOrders = 0;
        double m_rating = 5.0;
        std::vector<int> m_orderIds;
    };

    /**
     * @brief 车辆
     */
    class Vehicle {
    public:
        int id = 0;
        QString name;
        int speed = 2;
        int capacity = 30;
        double costPerKm = 0.3;

        [[nodiscard]] utils::json toJsonObject() const;
        static Vehicle fromJson(const utils::json& j);
    };

}
