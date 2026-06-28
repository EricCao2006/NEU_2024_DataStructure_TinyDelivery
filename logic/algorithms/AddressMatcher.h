#pragma once

#include "Algorithms.h"

#include <string>
#include <vector>
#include <utility>

namespace algo {

    /**
     * @brief 地址匹配（哈希表）
     * @details 用哈希表快速将地址名称映射到地图坐标
     */
    class AddressMatcher : public Algorithm {
    public:
        AddressMatcher() = default;
        std::string algorithmName() const override { return "哈希表(地址匹配)"; }

        struct Entry {
            std::string key;
            std::pair<int, int> position;
            bool occupied = false;
        };

        /**
         * @brief 构建哈希表（拉链法）
         * @param addresses 地址列表：(名称, x, y)
         * @param tableSize 哈希表桶数量
         */
        void buildTable(const std::vector<std::tuple<std::string, int, int>>& addresses,
                        int tableSize = 31);

        /**
         * @brief 根据地址名查找坐标
         * @return (x, y)，未找到返回 (-1, -1)
         */
        std::pair<int, int> lookup(const std::string& address) const;

        /**
         * @brief 向哈希表插入一条地址
         */
        void insert(const std::string& key, int x, int y);

        int getTableSize() const { return static_cast<int>(m_table.size()); }

    private:
        std::vector<std::vector<Entry>> m_table;

        unsigned long hashFunc(const std::string& key) const;
    };

}
