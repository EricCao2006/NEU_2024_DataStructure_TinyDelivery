#pragma once

#include <string>

namespace delivery {

    /**
     * @brief 客户类（订单接收方）
     */
    class Customer {
    public:
        Customer() = default;
        Customer(int id, const std::string& name, int x, int y, int vipLevel);

        // ========== Getter ==========
        [[nodiscard]] int id() const { return m_id; }
        [[nodiscard]] const std::string& name() const { return m_name; }
        [[nodiscard]] int x() const { return m_x; }
        [[nodiscard]] int y() const { return m_y; }
        [[nodiscard]] int vipLevel() const { return m_vipLevel; }                 ///< 0=普通, 1=银卡, 2=金卡, 3=钻石
        [[nodiscard]] int satisfaction() const { return m_satisfaction; }         ///< 满意度 0-100
        [[nodiscard]] int totalOrders() const { return m_totalOrders; }           ///< 累计订单数
        [[nodiscard]] double avgResponseTime() const { return m_avgResponseTime; } ///< 平均响应时间（秒）

        // ========== Setter ==========
        void setPosition(const int x, const int y) { m_x = x; m_y = y; }
        void setVipLevel(const int level) { m_vipLevel = level; }

        /**
         * @brief 记录一次配送完成
         * @param responseTime 本次响应时间（秒）
         * @param onTime 是否准时（影响满意度）
         */
        void recordDelivery(double responseTime, bool onTime);

        /**
         * @brief 获取VIP等级名称
         */
        [[nodiscard]] std::string vipLevelName() const;

        /**
         * @brief 该客户期望的最长等待时间（秒）
         */
        [[nodiscard]] int maxWaitTime() const;

        /**
         * @brief VIP客户对延迟的容忍度系数（越高越不能忍）
         */
        [[nodiscard]] double urgencyFactor() const;

    private:
        int m_id = -1;
        std::string m_name;
        int m_x = 0;
        int m_y = 0;
        int m_vipLevel = 0;
        int m_satisfaction = 80;                                     ///< 初始满意度 80
        int m_totalOrders = 0;
        double m_avgResponseTime = 0.0;
    };

} // namespace delivery