#include "Order.h"
#include <algorithm>

namespace delivery {

    Order::Order(int id, int merchantId, int customerId, double weight)
        : m_id(id)
        , m_merchantId(merchantId)
        , m_customerId(customerId)
        , m_weight(weight)
        , m_status(OrderStatus::PENDING)
        , m_deliverId(-1)
        , m_currentStep(0)
        , m_createTime(0)
        , m_priority(0) {}

    bool Order::advance() {
        if (m_route.empty()) {
            return false;
        }
        if (m_currentStep >= static_cast<int>(m_route.size()) - 1) {
            m_status = OrderStatus::COMPLETED;
            return true;
        }
        ++m_currentStep;
        return false;
    }

    std::pair<int, int> Order::currentPosition() const {
        if (m_route.empty() || m_currentStep >= static_cast<int>(m_route.size())) {
            return {-1, -1};
        }
        return m_route[m_currentStep];
    }

} // namespace delivery