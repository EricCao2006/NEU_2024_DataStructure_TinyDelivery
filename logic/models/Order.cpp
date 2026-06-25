#include "Order.h"
#include <algorithm>

namespace delivery {

    Order::Order(const int id, const int merchantId, const int customerId, const double weight)
        : m_id(id)
        , m_merchantId(merchantId)
        , m_customerId(customerId)
        , m_weight(weight) {}

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