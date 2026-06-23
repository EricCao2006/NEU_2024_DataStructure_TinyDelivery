#pragma once

#include <string>
#include <cstdint>

namespace delivery {

    /**
     * @brief 车辆类（骑手使用的交通工具）
     * @note 与 Deliver 中的 VehicleType 对应，但独立成类便于扩展属性
     */
    class Vehicle {
    public:
        Vehicle() = default;
        Vehicle(int id, const std::string& model, int speed, int capacity, double cost);

        // ========== Getter ==========
        int id() const { return m_id; }
        const std::string& model() const { return m_model; }
        int speed() const { return m_speed; }                       ///< 速度（格/秒）
        int capacity() const { return m_capacity; }                 ///< 载重（kg）
        double cost() const { return m_cost; }                      ///< 采购成本（万元）
        bool isOwned() const { return m_owned; }

        // ========== Setter ==========
        void setOwned(bool owned) { m_owned = owned; }

        /**
         * @brief 根据车辆类型创建预设车辆
         */
        static Vehicle createBicycle(int id);
        static Vehicle createVan(int id);
        static Vehicle createTruck(int id);

    private:
        int m_id = -1;
        std::string m_model;
        int m_speed = 0;
        int m_capacity = 0;
        double m_cost = 0.0;
        bool m_owned = false;
    };

} // namespace delivery