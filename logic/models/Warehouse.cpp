#include "Warehouse.h"
#include <algorithm>

namespace delivery {

    Warehouse::Warehouse(int id, const std::string& name, int x, int y, int capacity)
        : m_id(id)
        , m_name(name)
        , m_x(x)
        , m_y(y)
        , m_capacity(capacity) {}

    void Warehouse::upgrade() {
        if (m_level >= 5) return;
        ++m_level;
        m_capacity += 200 * m_level;
        m_throughput += 5.0 * m_level;
    }

    bool Warehouse::receive(const int count) {
        if (m_currentLoad + count > m_capacity) {
            return false;
        }
        m_currentLoad += count;
        return true;
    }

    int Warehouse::dispatch(const int count) {
        const int actual = std::min(count, m_currentLoad);
        m_currentLoad -= actual;
        return actual;
    }

    double Warehouse::loadRate() const {
        if (m_capacity == 0) return 1.0;
        return static_cast<double>(m_currentLoad) / m_capacity;
    }

    int Warehouse::upgradeCost() const {
        // 升级费用：等级越高越贵
        return 1000 + m_level * 800;
    }

} // namespace delivery