#pragma once

#include "Enums.h"
#include "../utils/json_.h"

namespace models {

    /**
     * @brief 订单
     */
    class Order {
    public:
        Order() = default;
        Order(int id, int merchantId, int customerId, int distance);

        [[nodiscard]] int getId() const { return m_id; }
        [[nodiscard]] int getMerchantId() const { return m_merchantId; }
        [[nodiscard]] int getCustomerId() const { return m_customerId; }
        [[nodiscard]] OrderStatus getStatus() const { return m_status; }
        [[nodiscard]] int getDistance() const { return m_distance; }
        [[nodiscard]] double getDeliveryFee() const { return m_deliveryFee; }
        [[nodiscard]] int getVipLevel() const { return m_vipLevel; }
        [[nodiscard]] int getCreateTime() const { return m_createTime; }
        [[nodiscard]] int getAssignTime() const { return m_assignTime; }
        [[nodiscard]] int getCompleteTime() const { return m_completeTime; }

        void setStatus(OrderStatus s) { m_status = s; }
        void setDeliveryFee(double fee) { m_deliveryFee = fee; }
        void setVipLevel(int level) { m_vipLevel = level; }
        void setCreateTime(int t) { m_createTime = t; }
        void setAssignTime(int t) { m_assignTime = t; }
        void setCompleteTime(int t) { m_completeTime = t; }

        /**
         * @brief 根据距离和VIP等级计算配送费
         */
        void calculateFee();

        [[nodiscard]] utils::json toJsonObject() const;
        static Order fromJson(const utils::json& j);

    private:
        int m_id = 0;
        int m_merchantId = 0;
        int m_customerId = 0;
        OrderStatus m_status = OrderStatus::Pending;
        int m_distance = 0;
        double m_deliveryFee = 0.0;
        int m_vipLevel = 0;
        int m_createTime = 0;
        int m_assignTime = 0;
        int m_completeTime = 0;
    };

}
