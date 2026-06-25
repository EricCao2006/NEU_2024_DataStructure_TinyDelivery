#include "Deliver.h"
#include <cmath>
#include <algorithm>

namespace delivery {

    Deliver::Deliver(const int id, const std::string& name, const int x, const int y, const VehicleType vehicle)
        : m_id(id)
        , m_name(name)
        , m_x(x)
        , m_y(y)
        , m_vehicle(vehicle) {}

    double Deliver::speedFactor() const {
        // 基础速度 × 经验加成 × 疲劳惩罚
        const double base = baseSpeed();
        const double expBonus = 1.0 + m_experience / 1000.0;   // 每1000经验 +10%
        double fatiguePenalty = 1.0 - m_fatigue / 200.0; // 疲劳100时速度减半
        fatiguePenalty = std::max(0.3, fatiguePenalty);
        return base * expBonus * fatiguePenalty;
    }

    void Deliver::rest() {
        m_fatigue = std::max(0, m_fatigue - 30);
        if (m_status == DeliverStatus::RESTING) {
            m_status = DeliverStatus::IDLE;
        }
    }

    bool Deliver::moveToward(const int tx, const int ty) {
        if (m_x == tx && m_y == ty) {
            return true;
        }
        // 曼哈顿移动：优先水平再垂直
        if (m_x < tx) ++m_x;
        else if (m_x > tx) --m_x;
        else if (m_y < ty) ++m_y;
        else if (m_y > ty) --m_y;

        // 每次移动增加疲劳
        m_fatigue = std::min(100, m_fatigue + 1);

        return m_x == tx && m_y == ty;
    }

    int Deliver::distanceTo(const int tx, const int ty) const {
        return std::abs(m_x - tx) + std::abs(m_y - ty);
    }

    double Deliver::baseSpeed() const {
        switch (m_vehicle) {
            case VehicleType::BICYCLE: return 1.5;
            case VehicleType::VAN:     return 2.5;
            case VehicleType::TRUCK:   return 3.5;
            default: return 1.0;
        }
    }

} // namespace delivery