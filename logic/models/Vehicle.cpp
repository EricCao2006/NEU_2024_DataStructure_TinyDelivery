#include "Vehicle.h"

namespace delivery {

    Vehicle::Vehicle(int id, const std::string& model, int speed, int capacity, double cost)
        : m_id(id)
        , m_model(model)
        , m_speed(speed)
        , m_capacity(capacity)
        , m_cost(cost)
        , m_owned(false) {}

    Vehicle Vehicle::createBicycle(int id) {
        return Vehicle(id, "电动单车", 2, 30, 0.3);
    }

    Vehicle Vehicle::createVan(int id) {
        return Vehicle(id, "面包车", 3, 200, 2.5);
    }

    Vehicle Vehicle::createTruck(int id) {
        return Vehicle(id, "冷链货车", 4, 500, 8.0);
    }

} // namespace delivery