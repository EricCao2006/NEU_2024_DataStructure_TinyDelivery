#include "Merchant.h"
#include <algorithm>

namespace delivery {

    Merchant::Merchant(int id, const std::string& name, int x, int y, MerchantType type)
        : m_id(id)
        , m_name(name)
        , m_x(x)
        , m_y(y)
        , m_type(type)
        , m_orderCount(0)
        , m_reputation(80)
        , m_active(true) {}

    void Merchant::changeReputation(int delta) {
        m_reputation = std::clamp(m_reputation + delta, 0, 100);
    }

    double Merchant::baseOrderInterval() const {
        // 返回间隔秒数
        switch (m_type) {
            case MerchantType::CAFE:      return 8.0;   // 高频
            case MerchantType::RETAIL:    return 15.0;
            case MerchantType::RESTAURANT:return 20.0;
            case MerchantType::FACTORY:   return 45.0;  // 低频
            case MerchantType::PHARMACY:  return 30.0;
            default: return 20.0;
        }
    }

    double Merchant::averageWeight() const {
        switch (m_type) {
            case MerchantType::CAFE:      return 0.5;
            case MerchantType::RETAIL:    return 2.0;
            case MerchantType::RESTAURANT:return 3.0;
            case MerchantType::FACTORY:   return 15.0;
            case MerchantType::PHARMACY:  return 1.0;
            default: return 2.0;
        }
    }

    std::string Merchant::typeName(MerchantType type) {
        switch (type) {
            case MerchantType::CAFE:      return "咖啡店";
            case MerchantType::RETAIL:    return "零售店";
            case MerchantType::RESTAURANT:return "餐厅";
            case MerchantType::FACTORY:   return "工厂";
            case MerchantType::PHARMACY:  return "药店";
            default: return "未知";
        }
    }

} // namespace delivery