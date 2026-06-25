#pragma once

#include <string>
#include <cstdint>

namespace delivery {

/**
 * @brief 骑手状态枚举
 */
enum class DeliverStatus : uint8_t {
    IDLE,          ///< 空闲
    PICKING,       ///< 前往取货
    DELIVERING,    ///< 派送中
    RESTING        ///< 休息中
};

/**
 * @brief 车辆类型枚举
 */
enum class VehicleType : uint8_t {
    BICYCLE,       ///< 电动单车（慢，便宜）
    VAN,           ///< 面包车（中）
    TRUCK          ///< 冷链货车（快，贵）
};

/**
 * @brief 骑手/快递员类
 */
class Deliver {
public:
    Deliver() = default;
    Deliver(int id, const std::string& name, int x, int y, VehicleType vehicle);

    // ========== Getter ==========
    [[nodiscard]] int id() const { return m_id; }
    [[nodiscard]] const std::string& name() const { return m_name; }
    [[nodiscard]] int x() const { return m_x; }
    [[nodiscard]] int y() const { return m_y; }
    [[nodiscard]] VehicleType vehicle() const { return m_vehicle; }
    [[nodiscard]] DeliverStatus status() const { return m_status; }
    [[nodiscard]] int currentOrderId() const { return m_currentOrderId; }
    [[nodiscard]] int experience() const { return m_experience; }            ///< 经验值，影响派送速度
    [[nodiscard]] int fatigue() const { return m_fatigue; }                  ///< 疲劳度 0-100，越高越慢
    [[nodiscard]] double speedFactor() const;                                 ///< 综合速度系数

    // ========== Setter ==========
    void setPosition(const int x, const int y) { m_x = x; m_y = y; }
    void setStatus(const DeliverStatus status) { m_status = status; }
    void setCurrentOrderId(const int orderId) { m_currentOrderId = orderId; }
    void addExperience(const int exp) { m_experience += exp; }
    void setFatigue(const int fatigue) { m_fatigue = fatigue; }
    void rest();                                                ///< 休息，降低疲劳

    /**
     * @brief 移动到目标坐标
     * @param tx 目标x
     * @param ty 目标y
     * @return 是否到达（即移动后坐标等于目标）
     */
    bool moveToward(int tx, int ty);

    /**
     * @brief 计算骑手到某点的曼哈顿距离
     */
    [[nodiscard]] int distanceTo(int tx, int ty) const;

    /**
     * @brief 获取车辆基础速度（格/秒）
     */
    [[nodiscard]] double baseSpeed() const;

private:
    int m_id = -1;
    std::string m_name;
    int m_x = 0;
    int m_y = 0;
    VehicleType m_vehicle = VehicleType::BICYCLE;
    DeliverStatus m_status = DeliverStatus::IDLE;
    int m_currentOrderId = -1;                                   ///< 当前配送的订单ID，-1表示无
    int m_experience = 0;
    int m_fatigue = 0;
};

} // namespace delivery