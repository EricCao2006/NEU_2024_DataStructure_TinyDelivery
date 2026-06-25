#pragma once

#include "models/Models.h"
#include "utils/json_.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace delivery {

/**
 * @brief 公司等级枚举
 */
enum class CompanyLevel : uint8_t {
    STREET_STALL,    ///< 街边快递站
    LOCAL_DEPOT,     ///< 同城物流点
    CITY_NETWORK,    ///< 城市物流网
    REGIONAL_HUB,    ///< 区域集散中心
    METRO_CHAIN      ///< 都市供应链
};

/**
 * @brief 公司等级信息
 */
struct LevelInfo {
    CompanyLevel level;          ///< 等级枚举
    std::string name;            ///< 显示名称
    int minReputation;           ///< 升级所需最低信誉
    int upgradeCost;             ///< 升级费用
    int maxDelivers;             ///< 最大骑手数
    int unlockWarehouseCount;    ///< 可解锁仓库数
};

/**
 * @brief 游戏全局状态
 * @note 单例模式，聚合所有游戏数据
 * @details 默认配置：
    等级：街边快递站
    资金：10000
    信誉：60
 */
class GameState {
public:
    static GameState& instance();

    // ========== 禁止拷贝 ==========
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;

    // ========== 存档 ==========
    /**
     * @brief 从JSON文件加载存档
     * @param path 文件路径
     * @return 是否成功
     */
    bool loadFromFile(const std::string& path);

    /**
     * @brief 保存到JSON文件
     * @param path 文件路径
     * @return 是否成功
     */
    bool saveToFile(const std::string& path) const;

    /**
     * @brief 重置为新游戏（从配置加载初始数据）
     */
    void reset();

    // ========== 资金 ==========
    int money() const { return m_money; }
    void addMoney(const int amount) { m_money += amount; }
    bool spendMoney(int amount);

    // ========== 时间 ==========
    int gameTime() const { return m_gameTime; }          ///< 游戏内时间（秒）
    void advanceTime(const int seconds) { m_gameTime += seconds; }

    // ========== 等级 ==========
    CompanyLevel level() const { return m_level; }
    int levelIndex() const { return static_cast<int>(m_level); }
    const LevelInfo& currentLevelInfo() const;
    bool canUpgrade() const;
    bool doUpgrade();

    // ========== 信誉 ==========
    int reputation() const { return m_reputation; }
    void changeReputation(int delta);
    static int maxReputation() { return 100; }

    // ========== 统计数据 ==========
    int totalOrders() const { return m_totalOrders; }
    int completedOrders() const { return m_completedOrders; }
    int failedOrders() const { return m_failedOrders; }
    double totalRevenue() const { return m_totalRevenue; }
    void recordOrderCompleted(double revenue);
    void recordOrderFailed();

    // ========== 数据容器 ==========
    std::vector<Order> orders;
    std::vector<Deliver> delivers;
    std::vector<Warehouse> warehouses;
    std::vector<Merchant> merchants;
    std::vector<Customer> customers;
    std::vector<Vehicle> vehicles;
    CityGrid cityGrid;

    // 等级配置（静态）
    static const std::vector<LevelInfo> LEVEL_CONFIG;
    
    // ========== 订单队列（待分配） ==========
    std::vector<int> pendingOrderIds;

    // ========== 索引映射（快速查找） ==========
    std::unordered_map<int, int> orderIndex;      ///< orderId -> vector索引
    std::unordered_map<int, int> deliverIndex;    ///< deliverId -> vector索引
    std::unordered_map<int, int> warehouseIndex;  ///< warehouseId -> vector索引
    std::unordered_map<int, int> merchantIndex;   ///< merchantId -> vector索引
    std::unordered_map<int, int> customerIndex;   ///< customerId -> vector索引

    // ========== 辅助方法 ==========
    Order* findOrder(int id);
    Deliver* findDeliver(int id);
    Warehouse* findWarehouse(int id);
    Merchant* findMerchant(int id);
    Customer* findCustomer(int id);

    const Order* findOrder(int id) const;
    const Deliver* findDeliver(int id) const;
    const Warehouse* findWarehouse(int id) const;
    const Merchant* findMerchant(int id) const;
    const Customer* findCustomer(int id) const;

    /**
     * @brief 获取当前可用的空闲骑手ID列表
     */
    std::vector<int> getIdleDelivers() const;

    /**
     * @brief 获取某个商家的所有待处理订单
     */
    std::vector<int> getPendingOrdersByMerchant(int merchantId) const;

private:
    GameState() = default;
    ~GameState() = default;

    // ========== 内部方法 ==========
    void rebuildIndex();
    void loadLevelConfig();
    void initDefaultCity();

    // ========== 状态成员 ==========
    int m_money = 10000;                         ///< 初始资金
    int m_gameTime = 0;                          ///< 游戏内时间（秒）
    CompanyLevel m_level = CompanyLevel::STREET_STALL;
    int m_reputation = 60;                       ///< 初始信誉

    // 统计
    int m_totalOrders = 0;
    int m_completedOrders = 0;
    int m_failedOrders = 0;
    double m_totalRevenue = 0.0;

};

} // namespace delivery