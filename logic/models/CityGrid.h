#pragma once

#include "Enums.h"
#include "Merchant.h"
#include "Customer.h"
#include "../utils/json_.h"

#include <QString>
#include <QPoint>
#include <vector>
#include <utility>

namespace models {

    /**
     * @brief 城市网格地图
     */
    class CityGrid {
    public:
        CityGrid() = default;

        explicit CityGrid(int size);

        [[nodiscard]] int getSize() const { return m_size; }

        /**
         * @brief 从JSON配置文件加载地图数据
         */
        void loadFromJSON(const utils::json& config);

        /**
         * @brief 获取指定坐标的地块类型
         */
        [[nodiscard]] TileType getTileType(int x, int y) const;

        /**
         * @brief 判断坐标是否可通行
         */
        [[nodiscard]] bool isWalkable(int x, int y) const;

        /**
         * @brief 判断坐标是否在地图范围内
         */
        [[nodiscard]] bool isInBounds(int x, int y) const;

        [[nodiscard]] const std::vector<Merchant>& getMerchants() const { return m_merchants; }
        [[nodiscard]] const std::vector<Customer>& getCustomers() const { return m_customers; }

        struct Warehouse {
            int id = 0;
            QString name;
            int x = 0, y = 0;
            int capacity = 0;
        };
        [[nodiscard]] const std::vector<Warehouse>& getWarehouses() const { return m_warehouses; }

        /**
         * @brief 根据ID查找商家/客户
         */
        [[nodiscard]] const Merchant* findMerchant(int id) const;
        [[nodiscard]] const Customer* findCustomer(int id) const;

        /**
         * @brief 网格坐标 → 屏幕像素坐标（格子中心）
         */
        [[nodiscard]] QPointF gridToScreen(int gridX, int gridY, int tileSize) const;

        /**
         * @brief 屏幕像素坐标 → 网格坐标
         */
        [[nodiscard]] QPoint screenToGrid(double screenX, double screenY, int tileSize) const;

    private:
        int m_size = 20;
        std::vector<Merchant> m_merchants;
        std::vector<Customer> m_customers;
        std::vector<Warehouse> m_warehouses;
        std::vector<std::pair<int, int>> m_obstacles;
    };

}
