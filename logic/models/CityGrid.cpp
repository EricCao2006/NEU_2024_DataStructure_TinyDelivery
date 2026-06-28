#include "CityGrid.h"
#include "../utils/log_.h"
#include "../utils/math_.h"

namespace models {

    CityGrid::CityGrid(const int size) : m_size(size) {}

    void CityGrid::loadFromJSON(const utils::json& config) {
        m_size = config.value("grid_size", 20);

        // 加载商家
        m_merchants.clear();
        if (config.contains("merchants") && config["merchants"].is_array()) {
            for (const auto& j : config["merchants"]) {
                m_merchants.push_back(Merchant::fromJson(j));
            }
        }

        // 加载客户
        m_customers.clear();
        if (config.contains("customers") && config["customers"].is_array()) {
            for (const auto& j : config["customers"]) {
                m_customers.push_back(Customer::fromJson(j));
            }
        }

        // 加载仓库
        m_warehouses.clear();
        if (config.contains("warehouses") && config["warehouses"].is_array()) {
            for (const auto& j : config["warehouses"]) {
                Warehouse w;
                w.id = j.value("id", 0);
                w.name = QString::fromStdString(j.value("name", ""));
                w.x = j.value("x", 0);
                w.y = j.value("y", 0);
                w.capacity = j.value("capacity", 0);
                m_warehouses.push_back(w);
            }
        }

        // 加载障碍物
        m_obstacles.clear();
        if (config.contains("obstacles") && config["obstacles"].is_array()) {
            for (const auto& j : config["obstacles"]) {
                int ox = j.value("x", 0);
                int oy = j.value("y", 0);
                m_obstacles.emplace_back(ox, oy);
            }
        }

        utils::Logger::log(1, "CityGrid loaded: " + std::to_string(m_merchants.size()) + " merchants, "
                           + std::to_string(m_customers.size()) + " customers, "
                           + std::to_string(m_obstacles.size()) + " obstacles");
    }

    TileType CityGrid::getTileType(const int x, const int y) const {
        if (!isInBounds(x, y)) {
            return TileType::Obstacle;  // 地图外视为障碍
        }

        // 检查障碍物
        for (const auto& [ox, oy] : m_obstacles) {
            if (ox == x && oy == y) return TileType::Obstacle;
        }

        // 检查商家位置
        for (const auto& m : m_merchants) {
            if (m.getX() == x && m.getY() == y) return TileType::Merchant;
        }

        // 检查客户位置
        for (const auto& c : m_customers) {
            if (c.getX() == x && c.getY() == y) return TileType::Customer;
        }

        // 检查仓库位置
        for (const auto& w : m_warehouses) {
            if (w.x == x && w.y == y) return TileType::Warehouse;
        }

        return TileType::Road;
    }

    bool CityGrid::isWalkable(const int x, const int y) const {
        if (!isInBounds(x, y)) return false;
        const TileType tile = getTileType(x, y);
        // 道路、商家、客户、仓库都可通行；只有障碍不可通行
        return tile != TileType::Obstacle;
    }

    bool CityGrid::isInBounds(const int x, const int y) const {
        return x >= 0 && x < m_size && y >= 0 && y < m_size;
    }

    const Merchant* CityGrid::findMerchant(const int id) const {
        for (const auto& m : m_merchants) {
            if (m.getId() == id) return &m;
        }
        return nullptr;
    }

    const Customer* CityGrid::findCustomer(const int id) const {
        for (const auto& c : m_customers) {
            if (c.getId() == id) return &c;
        }
        return nullptr;
    }

    QPointF CityGrid::gridToScreen(const int gridX, const int gridY, const int tileSize) const {
        return {gridX * tileSize + tileSize / 2.0,
                       gridY * tileSize + tileSize / 2.0};
    }

    QPoint CityGrid::screenToGrid(const double screenX, const double screenY, const int tileSize) const {
        const int gx = static_cast<int>(screenX / tileSize);
        const int gy = static_cast<int>(screenY / tileSize);
        return {utils::clamp(gx, 0, m_size - 1),
                      utils::clamp(gy, 0, m_size - 1)};
    }

}
