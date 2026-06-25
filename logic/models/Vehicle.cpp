#include "Vehicle.h"

namespace delivery {

    Vehicle::Vehicle(const int id, const std::string& model, const int speed, const int capacity, const double cost)
        : m_id(id)
        , m_model(model)
        , m_speed(speed)
        , m_capacity(capacity)
        , m_cost(cost)
        , m_owned(false) {}

    Vehicle Vehicle::createBicycle(const int id) {
        return Vehicle(id, "电动单车", 2, 30, 0.3);
    }

    Vehicle Vehicle::createVan(const int id) {
        return Vehicle(id, "面包车", 3, 200, 2.5);
    }

    Vehicle Vehicle::createTruck(const int id) {
        return Vehicle(id, "冷链货车", 4, 500, 8.0);
    }

} // namespace delivery