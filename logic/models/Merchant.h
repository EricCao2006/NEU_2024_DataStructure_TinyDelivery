#pragma once

#include <string>
#include <cstdint>

namespace delivery {

    /**
     * @brief 商家类型枚举
     */
    enum class MerchantType : uint8_t {
        CAFE,          ///< 咖啡店（高频小单）
        RETAIL,        ///< 零售店（中频中单）
        RESTAURANT,    ///< 餐厅（高峰集中）
        FACTORY,       ///< 工厂（低频大单）
        PHARMACY       ///< 药店（低频急单）
    };

    /**
     * @brief 商家类（订单生产方）
     */
    class Merchant {
    public:
        Merchant() = default;
        Merchant(int id, const std::string& name, int x, int y, MerchantType type);

        // ========== Getter ==========
        int id() const { return m_id; }
        const std::string& name() const { return m_name; }
        int x() const { return m_x; }
        int y() const { return m_y; }
        MerchantType type() const { return m_type; }
        int orderCount() const { return m_orderCount; }             ///< 累计发出订单数
        int reputation() const { return m_reputation; }             ///< 商家信誉 0-100
        bool isActive() const { return m_active; }                  ///< 是否营业中

        // ========== Setter ==========
        void setPosition(int x, int y) { m_x = x; m_y = y; }
        void setActive(bool active) { m_active = active; }
        void addOrder() { ++m_orderCount; }
        void changeReputation(int delta);

        /**
         * @brief 获取该商家的基础出单间隔（秒）
         * @return 间隔秒数，越小出单越频繁
         */
        double baseOrderInterval() const;

        /**
         * @brief 获取该商家的平均订单重量（kg）
         */
        double averageWeight() const;

        /**
         * @brief 获取商家类型名称（中文）
         */
        static std::string typeName(MerchantType type);

    private:
        int m_id = -1;
        std::string m_name;
        int m_x = 0;
        int m_y = 0;
        MerchantType m_type = MerchantType::RETAIL;
        int m_orderCount = 0;
        int m_reputation = 80;                                       ///< 初始信誉 80
        bool m_active = true;
    };

} // namespace delivery