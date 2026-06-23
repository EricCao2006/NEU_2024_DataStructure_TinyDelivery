#include "AddressMatcher.h"
#include <cmath>

namespace delivery::algo {

// ========== 构造函数 ==========
AddressMatcher::AddressMatcher(size_t initialCapacity, double maxLoadFactor)
    : m_capacity(initialCapacity)
    , m_maxLoadFactor(maxLoadFactor) {
    m_table.resize(m_capacity);
}

// ========== 哈希函数 ==========
uint64_t AddressMatcher::polynomialHash(const std::string& key, uint64_t seed) {
    // 使用 64 位多项式滚动哈希
    // 选取较大的质数作为基数
    const uint64_t BASE = 0x9e3779b97f4a7c15ULL;
    uint64_t hash = seed;

    for (char c : key) {
        hash = (hash * BASE) ^ static_cast<uint64_t>(c);
        hash ^= hash >> 33;
        hash *= BASE;
        hash ^= hash >> 29;
        hash *= BASE;
        hash ^= hash >> 31;
    }

    return hash;
}

// ========== 索引计算 ==========
size_t AddressMatcher::getSlotIndex(uint64_t hash) const {
    // 容量为2的幂时，用位运算替代取模
    return hash & (m_capacity - 1);
}

// ========== 探测 ==========
bool AddressMatcher::probe(const std::string& key, size_t& outIndex) const {
    uint64_t hash = polynomialHash(key);
    size_t index = getSlotIndex(hash);
    size_t start = index;

    for (size_t i = 0; i < m_capacity; ++i) {
        const auto& slot = m_table[index];
        if (slot.state == SlotState::EMPTY) {
            return false;
        }
        if (slot.state == SlotState::OCCUPIED && slot.key == key) {
            outIndex = index;
            return true;
        }
        index = (index + 1) & (m_capacity - 1);  // 线性探测
    }
    return false;
}

bool AddressMatcher::probeForInsert(const std::string& key, size_t& outIndex) const {
    uint64_t hash = polynomialHash(key);
    size_t index = getSlotIndex(hash);
    size_t firstTombstone = m_capacity;  // 记录第一个墓碑位置

    for (size_t i = 0; i < m_capacity; ++i) {
        const auto& slot = m_table[index];
        if (slot.state == SlotState::EMPTY) {
            if (firstTombstone != m_capacity) {
                outIndex = firstTombstone;
            } else {
                outIndex = index;
            }
            return true;
        }
        if (slot.state == SlotState::TOMBSTONE) {
            if (firstTombstone == m_capacity) {
                firstTombstone = index;
            }
        } else if (slot.state == SlotState::OCCUPIED && slot.key == key) {
            outIndex = index;
            return true;  // 更新现有键
        }
        index = (index + 1) & (m_capacity - 1);
    }

    // 完全满了（不应该发生，因为有负载因子控制）
    if (firstTombstone != m_capacity) {
        outIndex = firstTombstone;
        return true;
    }
    return false;
}

// ========== 核心操作 ==========
bool AddressMatcher::insert(const std::string& key, int value) {
    checkLoadFactor();

    size_t index;
    if (!probeForInsert(key, index)) {
        return false;
    }

    bool isNew = (m_table[index].state != SlotState::OCCUPIED || m_table[index].key != key);
    if (isNew) {
        ++m_size;
    }

    m_table[index].key = key;
    m_table[index].value = value;
    m_table[index].state = SlotState::OCCUPIED;
    return true;
}

bool AddressMatcher::find(const std::string& key, int& outValue) const {
    size_t index;
    if (!probe(key, index)) {
        return false;
    }
    outValue = m_table[index].value;
    return true;
}

bool AddressMatcher::remove(const std::string& key) {
    size_t index;
    if (!probe(key, index)) {
        return false;
    }
    m_table[index].state = SlotState::TOMBSTONE;
    --m_size;
    return true;
}

void AddressMatcher::clear() {
    for (auto& slot : m_table) {
        slot.state = SlotState::EMPTY;
        slot.key.clear();
        slot.value = 0;
    }
    m_size = 0;
}

// ========== 负载管理 ==========
void AddressMatcher::checkLoadFactor() {
    double load = static_cast<double>(m_size) / m_capacity;
    if (load >= m_maxLoadFactor) {
        rehash(m_capacity * 2);
    }
}

void AddressMatcher::rehash(size_t newCapacity) {
    // 保存旧表
    std::vector<HashSlot> oldTable = std::move(m_table);
    size_t oldCapacity = m_capacity;

    // 创建新表
    m_capacity = newCapacity;
    m_table.resize(m_capacity);
    m_size = 0;

    // 重新插入所有元素
    for (size_t i = 0; i < oldCapacity; ++i) {
        if (oldTable[i].state == SlotState::OCCUPIED) {
            insert(oldTable[i].key, oldTable[i].value);
        }
    }
}

} // namespace delivery::algo