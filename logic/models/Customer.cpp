#include "Customer.h"
#include <algorithm>
#include <string>

namespace delivery {

    Customer::Customer(int id, const std::string& name, int x, int y, int vipLevel)
        : m_id(id)
        , m_name(name)
        , m_x(x)
        , m_y(y)
        , m_vipLevel(std::clamp(vipLevel, 0, 3))
        , m_satisfaction(80)
        , m_totalOrders(0)
        , m_avgResponseTime(0.0) {}

    void Customer::recordDelivery(double responseTime, bool onTime) {
        ++m_totalOrders;
        // 更新平均响应时间（指数移动平均）
        m_avgResponseTime = (m_avgResponseTime * (m_totalOrders - 1) + responseTime) / m_totalOrders;

        // 满意度变化
        int delta = 0;
        if (onTime) {
            delta = 2 + m_vipLevel;  // VIP客户按时送达加分更多
        } else {
            // 超时惩罚 = 基础惩罚 × VIP紧迫系数
            double penalty = 5.0 * (1.0 + m_vipLevel * 0.5);
            delta = -static_cast<int>(penalty);
        }
        m_satisfaction = std::clamp(m_satisfaction + delta, 0, 100);
    }

    std::string Customer::vipLevelName() const {
        switch (m_vipLevel) {
            case 0: return "普通";
            case 1: return "银卡";
            case 2: return "金卡";
            case 3: return "钻石";
            default: return "普通";
        }
    }

    int Customer::maxWaitTime() const {
        // VIP等级越高，期望越快
        switch (m_vipLevel) {
            case 0: return 1800;  // 30分钟
            case 1: return 1200;  // 20分钟
            case 2: return 900;   // 15分钟
            case 3: return 600;   // 10分钟
            default: return 1800;
        }
    }

    double Customer::urgencyFactor() const {
        // VIP越高越不能容忍延迟
        return 1.0 + m_vipLevel * 0.4;
    }

} // namespace delivery