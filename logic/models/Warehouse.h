#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace delivery {

    /**
     * @brief 仓库类，中转枢纽
     */
    class Warehouse {
    public:
        Warehouse() = default;
        Warehouse(int id, const std::string& name, int x, int y, int capacity);

        // ========== Getter ==========
        int id() const { return m_id; }
        const std::string& name() const { return m_name; }
        int x() const { return m_x; }
        int y() const { return m_y; }
        int capacity() const { return m_capacity; }
        int currentLoad() const { return m_currentLoad; }
        double throughput() const { return m_throughput; }          ///< 吞吐量（件/秒）
        int level() const { return m_level; }                       ///< 仓库等级 1-5

        // ========== Setter ==========
        void setPosition(int x, int y) { m_x = x; m_y = y; }
        void upgrade();                                              ///< 升级仓库（容量+吞吐量提升）
        void setThroughput(double throughput) { m_throughput = throughput; }

        /**
         * @brief 入库
         * @param count 件数
         * @return 是否成功（爆仓则失败）
         */
        bool receive(int count);

        /**
         * @brief 出库
         * @param count 件数
         * @return 实际出库件数（不足则全部出库）
         */
        int dispatch(int count);

        /**
         * @brief 获取当前负载率 (0-1)
         */
        double loadRate() const;

        /**
         * @brief 是否爆仓（负载率 >= 1）
         */
        bool isFull() const { return m_currentLoad >= m_capacity; }

        /**
         * @brief 获取升级到下一级的费用
         */
        int upgradeCost() const;

    private:
        int m_id = -1;
        std::string m_name;
        int m_x = 0;
        int m_y = 0;
        int m_capacity = 500;                                        ///< 最大容量（件）
        int m_currentLoad = 0;                                       ///< 当前库存
        double m_throughput = 10.0;                                  ///< 吞吐量（件/秒）
        int m_level = 1;
    };

} // namespace delivery