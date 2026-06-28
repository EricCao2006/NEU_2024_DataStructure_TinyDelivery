#include "AddressMatcher.h"

namespace algo {

    unsigned long AddressMatcher::hashFunc(const std::string& key) const {
        // BKDR Hash
        unsigned long hash = 0;
        for (const char ch : key) {
            constexpr unsigned long seed = 131;
            hash = hash * seed + ch;
        }
        return hash;
    }

    void AddressMatcher::buildTable(
        const std::vector<std::tuple<std::string, int, int>>& addresses,
        const int tableSize)
    {
        m_table.clear();
        m_table.resize(tableSize);

        for (const auto& [name, x, y] : addresses) {
            insert(name, x, y);
        }
    }

    std::pair<int, int> AddressMatcher::lookup(const std::string& address) const {
        callCount++;
        if (m_table.empty()) return {-1, -1};

        const unsigned long idx = hashFunc(address) % m_table.size();
        for (const auto&[key, position, occupied] : m_table[idx]) {
            if (occupied && key == address) {
                return position;
            }
            count++;
        }
        return {-1, -1};
    }

    void AddressMatcher::insert(const std::string& key, int x, int y) {
        if (m_table.empty()) return;

        const unsigned long idx = hashFunc(key) % m_table.size();
        // 检查是否已存在
        for (auto& entry : m_table[idx]) {
            if (entry.occupied && entry.key == key) {
                entry.position = {x, y};
                return;
            }
        }
        // 拉链法：追加到链尾
        m_table[idx].push_back({key, {x, y}, true});
    }

}
