#pragma once
/**
 * @file AddressMatcher.h
 * @brief 地址哈希匹配算法
 * @details 基于哈希表实现的高速地址→ID查找
 *          使用多项式滚动哈希 + 开放定址法解决冲突
 */

#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace delivery::algo {

/**
 * @brief 哈希表槽位状态
 */
enum class SlotState : uint8_t {
    EMPTY,       ///< 空槽
    OCCUPIED,    ///< 已占用
    TOMBSTONE    ///< 已删除（标记）
};

/**
 * @brief 哈希表槽位
 */
struct HashSlot {
    std::string key;            ///< 地址字符串
    int value;                  ///< 对应的ID
    SlotState state = SlotState::EMPTY;
};

/**
 * @brief 地址哈希表
 * @details 使用开放定址法（线性探测）解决冲突
 */
class AddressMatcher {
public:
    /**
     * @param initialCapacity 初始容量（必须是2的幂）
     * @param maxLoadFactor 最大负载因子，超过则扩容
     */
    AddressMatcher(size_t initialCapacity = 1024, double maxLoadFactor = 0.7);
    ~AddressMatcher() = default;

    /**
     * @brief 插入或更新键值对
     * @param key 地址字符串
     * @param value 对应的ID
     * @return true 成功插入/更新
     */
    bool insert(const std::string& key, int value);

    /**
     * @brief 查找键对应的值
     * @param key 地址字符串
     * @param outValue 输出值
     * @return true 找到
     */
    bool find(const std::string& key, int& outValue) const;

    /**
     * @brief 删除键
     * @param key 地址字符串
     * @return true 删除成功
     */
    bool remove(const std::string& key);

    /**
     * @brief 获取当前元素数量
     */
    size_t size() const { return m_size; }

    /**
     * @brief 获取当前容量
     */
    size_t capacity() const { return m_capacity; }

    /**
     * @brief 清空哈希表
     */
    void clear();

private:
    /**
     * @brief 多项式滚动哈希
     * @param key 输入字符串
     * @param seed 随机种子
     * @return 64位哈希值
     */
    static uint64_t polynomialHash(const std::string& key, uint64_t seed = 0x9e3779b97f4a7c15ULL);

    /**
     * @brief 计算槽位索引
     */
    size_t getSlotIndex(uint64_t hash) const;

    /**
     * @brief 线性探测找槽位
     * @param key 要查找的键
     * @param outIndex 输出找到的槽位索引
     * @return true 找到
     */
    bool probe(const std::string& key, size_t& outIndex) const;

    /**
     * @brief 探测空槽位（用于插入）
     * @param key 要插入的键
     * @param outIndex 输出可用的槽位索引
     * @return true 找到可用槽位
     */
    bool probeForInsert(const std::string& key, size_t& outIndex) const;

    /**
     * @brief 扩容到新容量
     */
    void rehash(size_t newCapacity);

    /**
     * @brief 检查负载因子并自动扩容
     */
    void checkLoadFactor();

    std::vector<HashSlot> m_table;
    size_t m_size = 0;              ///< 实际元素数
    size_t m_capacity = 1024;       ///< 当前容量
    double m_maxLoadFactor = 0.7;
};

} // namespace delivery::algo