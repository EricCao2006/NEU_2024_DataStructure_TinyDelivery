#include "GameState.h"
#include "utils/log_.h"
#include "utils/random_.h"
#include <algorithm>
#include <fstream>

namespace delivery {

// ========== 等级配置 ==========
const std::vector<LevelInfo> GameState::LEVEL_CONFIG = {
    {CompanyLevel::STREET_STALL,  "街边快递站",  0,    0,   5,  1},
    {CompanyLevel::LOCAL_DEPOT,   "同城物流点",  30,  5000, 10, 2},
    {CompanyLevel::CITY_NETWORK,  "城市物流网",  50,  15000, 20, 3},
    {CompanyLevel::REGIONAL_HUB,  "区域集散中心",70,  40000, 35, 4},
    {CompanyLevel::METRO_CHAIN,   "都市供应链",  90,  100000, 50, 5}
};

// ========== 单例 ==========
GameState& GameState::instance() {
    static GameState instance;
    return instance;
}

// ========== 存档 ==========
bool GameState::loadFromFile(const std::string& path) {
    auto json = utils::loadJSON(path);
    if (json.empty()) {
        LOG_WARN("存档文件不存在或为空，使用默认初始化: " + path);
        reset();
        return false;
    }

    try {
        // 基本状态
        m_money = json.value("money", 10000);
        m_gameTime = json.value("gameTime", 0);
        m_level = static_cast<CompanyLevel>(json.value("level", 0));
        m_reputation = json.value("reputation", 60);
        m_totalOrders = json.value("totalOrders", 0);
        m_completedOrders = json.value("completedOrders", 0);
        m_failedOrders = json.value("failedOrders", 0);
        m_totalRevenue = json.value("totalRevenue", 0.0);

        // 加载各数据容器
        // 清空现有数据
        orders.clear();
        delivers.clear();
        warehouses.clear();
        merchants.clear();
        customers.clear();
        vehicles.clear();
        pendingOrderIds.clear();

        // 加载商家
        if (json.contains("merchants")) {
            for (const auto& m : json["merchants"]) {
                Merchant merchant(
                    m.value("id", 0),
                    m.value("name", "商家"),
                    m.value("x", 0),
                    m.value("y", 0),
                    static_cast<MerchantType>(m.value("type", 1))
                );
                merchant.setActive(m.value("active", true));
                merchant.changeReputation(m.value("reputation", 80) - 80);
                // 恢复订单计数
                for (int i = 0; i < m.value("orderCount", 0); ++i) {
                    merchant.addOrder();
                }
                merchants.push_back(merchant);
            }
        }

        // 加载客户
        if (json.contains("customers")) {
            for (const auto& c : json["customers"]) {
                Customer customer(
                    c.value("id", 0),
                    c.value("name", "客户"),
                    c.value("x", 0),
                    c.value("y", 0),
                    c.value("vipLevel", 0)
                );
                customer.setVipLevel(c.value("vipLevel", 0));
                // 恢复满意度（通过记录模拟）
                if (int sat = c.value("satisfaction", 80); sat > 80) {
                    for (int i = 0; i < sat - 80; ++i) {
                        customer.recordDelivery(100, true);
                    }
                } else if (sat < 80) {
                    for (int i = 0; i < 80 - sat; ++i) {
                        customer.recordDelivery(2000, false);
                    }
                }
                customers.push_back(customer);
            }
        }

        // 加载仓库
        if (json.contains("warehouses")) {
            for (const auto& w : json["warehouses"]) {
                Warehouse warehouse(
                    w.value("id", 0),
                    w.value("name", "仓库"),
                    w.value("x", 0),
                    w.value("y", 0),
                    w.value("capacity", 500)
                );
                // 恢复库存（通过多次 receive）
                int load = w.value("currentLoad", 0);
                for (int i = 0; i < load; ++i) {
                    warehouse.receive(1);
                }
                // 恢复等级
                int level = w.value("level", 1);
                for (int i = 1; i < level; ++i) {
                    warehouse.upgrade();
                }
                warehouses.push_back(warehouse);
            }
        }

        // 加载骑手
        if (json.contains("delivers")) {
            for (const auto& d : json["delivers"]) {
                auto vt = static_cast<VehicleType>(d.value("vehicle", 0));
                Deliver deliver(
                    d.value("id", 0),
                    d.value("name", "骑手"),
                    d.value("x", 0),
                    d.value("y", 0),
                    vt
                );
                deliver.addExperience(d.value("experience", 0));
                deliver.setFatigue(d.value("fatigue", 0));
                delivers.push_back(deliver);
            }
        }

        // 加载订单（简化：只恢复已完成的订单）
        if (json.contains("orders")) {
            for (const auto& o : json["orders"]) {
                Order order(
                    o.value("id", 0),
                    o.value("merchantId", 0),
                    o.value("customerId", 0),
                    o.value("weight", 1.0)
                );
                order.setCreateTime(o.value("createTime", 0));
                order.setPriority(o.value("priority", 0));
                // 只恢复已完成订单
                if (o.value("status", 0) == static_cast<int>(OrderStatus::COMPLETED)) {
                    order.setStatus(OrderStatus::COMPLETED);
                    orders.push_back(order);
                }
                // 待处理订单在 pendingOrderIds 中恢复
            }
        }

        // 加载待处理订单ID
        if (json.contains("pendingOrderIds")) {
            for (const auto& id : json["pendingOrderIds"]) {
                pendingOrderIds.push_back(id.get<int>());
            }
        }

        // 加载城市网格
        if (json.contains("cityGrid")) {
            int size = json["cityGrid"].value("size", 20);
            cityGrid = CityGrid(size);
            if (json["cityGrid"].contains("obstacles")) {
                for (const auto& obs : json["cityGrid"]["obstacles"]) {
                    cityGrid.addObstacle(obs.value("x", 0), obs.value("y", 0));
                }
            }
        }

        rebuildIndex();
        LOG_INFO("存档加载成功: " + path);
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("加载存档失败: " + std::string(e.what()));
        reset();
        return false;
    }
}

bool GameState::saveToFile(const std::string& path) const {
    utils::json j;

    // 基本状态
    j["money"] = m_money;
    j["gameTime"] = m_gameTime;
    j["level"] = m_level;
    j["reputation"] = m_reputation;
    j["totalOrders"] = m_totalOrders;
    j["completedOrders"] = m_completedOrders;
    j["failedOrders"] = m_failedOrders;
    j["totalRevenue"] = m_totalRevenue;

    // ★ 序列化各数据容器

    // 序列化商家
    utils::json merchantsArr = utils::json::array();
    for (const auto& m : merchants) {
        utils::json obj;
        obj["id"] = m.id();
        obj["name"] = m.name();
        obj["x"] = m.x();
        obj["y"] = m.y();
        obj["type"] = m.type();
        obj["active"] = m.isActive();
        obj["reputation"] = m.reputation();
        obj["orderCount"] = m.orderCount();
        merchantsArr.push_back(obj);
    }
    j["merchants"] = merchantsArr;

    // 序列化客户
    utils::json customersArr = utils::json::array();
    for (const auto& c : customers) {
        utils::json obj;
        obj["id"] = c.id();
        obj["name"] = c.name();
        obj["x"] = c.x();
        obj["y"] = c.y();
        obj["vipLevel"] = c.vipLevel();
        obj["satisfaction"] = c.satisfaction();
        customersArr.push_back(obj);
    }
    j["customers"] = customersArr;

    // 序列化仓库
    utils::json warehousesArr = utils::json::array();
    for (const auto& w : warehouses) {
        utils::json obj;
        obj["id"] = w.id();
        obj["name"] = w.name();
        obj["x"] = w.x();
        obj["y"] = w.y();
        obj["capacity"] = w.capacity();
        obj["currentLoad"] = w.currentLoad();
        obj["level"] = w.level();
        warehousesArr.push_back(obj);
    }
    j["warehouses"] = warehousesArr;

    // 序列化骑手
    utils::json deliversArr = utils::json::array();
    for (const auto& d : delivers) {
        utils::json obj;
        obj["id"] = d.id();
        obj["name"] = d.name();
        obj["x"] = d.x();
        obj["y"] = d.y();
        obj["vehicle"] = d.vehicle();
        obj["experience"] = d.experience();
        obj["fatigue"] = d.fatigue();
        deliversArr.push_back(obj);
    }
    j["delivers"] = deliversArr;

    // 序列化订单（只存已完成和待处理的）
    utils::json ordersArr = utils::json::array();
    for (const auto& o : orders) {
        // 只保存已完成或待处理的订单
        if (o.status() == OrderStatus::COMPLETED || o.status() == OrderStatus::PENDING) {
            utils::json obj;
            obj["id"] = o.id();
            obj["merchantId"] = o.merchantId();
            obj["customerId"] = o.customerId();
            obj["weight"] = o.weight();
            obj["status"] = o.status();
            obj["createTime"] = o.createTime();
            obj["priority"] = o.priority();
            ordersArr.push_back(obj);
        }
    }
    j["orders"] = ordersArr;

    // 序列化待处理订单ID
    j["pendingOrderIds"] = pendingOrderIds;

    // 序列化城市网格
    utils::json gridObj;
    gridObj["size"] = cityGrid.size();

    // 保存城市障碍物
    utils::json obstaclesArr = utils::json::array();
    for (const auto&[fst, snd] : cityGrid.getObstacles()) {
        utils::json obj;
        obj["x"] = fst;
        obj["y"] = snd;
        obstaclesArr.push_back(obj);
    }
    j["cityGrid"]["obstacles"] = obstaclesArr;

    bool ok = utils::saveJSON(path, j);
    if (ok) {
        LOG_INFO("存档保存成功: " + path);
    } else {
        LOG_ERROR("存档保存失败: " + path);
    }
    return ok;
}

void GameState::reset() {
    m_money = 10000;
    m_gameTime = 0;
    m_level = CompanyLevel::STREET_STALL;
    m_reputation = 60;
    m_totalOrders = 0;
    m_completedOrders = 0;
    m_failedOrders = 0;
    m_totalRevenue = 0.0;

    orders.clear();
    delivers.clear();
    warehouses.clear();
    merchants.clear();
    customers.clear();
    vehicles.clear();
    pendingOrderIds.clear();

    initDefaultCity();
    rebuildIndex();
    LOG_INFO("游戏已重置");
}

// ========== 资金 ==========
bool GameState::spendMoney(const int amount) {
    if (amount < 0) {
        LOG_WARN("spendMoney: 金额不能为负数");
        return false;
    }
    if (m_money < amount) {
        return false;
    }
    m_money -= amount;
    return true;
}

// ========== 等级 ==========
const LevelInfo& GameState::currentLevelInfo() const {
    const int idx = static_cast<int>(m_level);
    if (idx < 0 || idx >= static_cast<int>(LEVEL_CONFIG.size())) {
        return LEVEL_CONFIG[0];
    }
    return LEVEL_CONFIG[idx];
}

bool GameState::canUpgrade() const {
    const int idx = static_cast<int>(m_level);
    if (idx >= static_cast<int>(LEVEL_CONFIG.size()) - 1) {
        return false;  // 已是最高级
    }
    const auto& next = LEVEL_CONFIG[idx + 1];
    return m_reputation >= next.minReputation && m_money >= next.upgradeCost;
}

bool GameState::doUpgrade() {
    if (!canUpgrade()) {
        LOG_WARN("升级条件不满足");
        return false;
    }
    const int idx = static_cast<int>(m_level);
    const auto& next = LEVEL_CONFIG[idx + 1];
    if (!spendMoney(next.upgradeCost)) {
        return false;
    }
    m_level = next.level;
    LOG_INFO("公司升级到: " + next.name);
    return true;
}

// ========== 信誉 ==========
void GameState::changeReputation(const int delta) {
    m_reputation = std::clamp(m_reputation + delta, 0, 100);
    if (delta != 0) {
        LOG_DEBUG("信誉变化: " + std::to_string(delta) + ", 当前: " + std::to_string(m_reputation));
    }
}

// ========== 统计 ==========
void GameState::recordOrderCompleted(const double revenue) {
    ++m_completedOrders;
    ++m_totalOrders;
    m_totalRevenue += revenue;
    addMoney(static_cast<int>(revenue));
    // 按时完成提升信誉
    changeReputation(1);
}

void GameState::recordOrderFailed() {
    ++m_failedOrders;
    ++m_totalOrders;
    changeReputation(-3);
}

// ========== 查找 ==========
Order* GameState::findOrder(const int id) {
    const auto it = orderIndex.find(id);
    if (it == orderIndex.end() || it->second >= static_cast<int>(orders.size())) {
        return nullptr;
    }
    return &orders[it->second];
}

Deliver* GameState::findDeliver(const int id) {
    const auto it = deliverIndex.find(id);
    if (it == deliverIndex.end() || it->second >= static_cast<int>(delivers.size())) {
        return nullptr;
    }
    return &delivers[it->second];
}

Warehouse* GameState::findWarehouse(const int id) {
    const auto it = warehouseIndex.find(id);
    if (it == warehouseIndex.end() || it->second >= static_cast<int>(warehouses.size())) {
        return nullptr;
    }
    return &warehouses[it->second];
}

Merchant* GameState::findMerchant(const int id) {
    const auto it = merchantIndex.find(id);
    if (it == merchantIndex.end() || it->second >= static_cast<int>(merchants.size())) {
        return nullptr;
    }
    return &merchants[it->second];
}

Customer* GameState::findCustomer(const int id) {
    const auto it = customerIndex.find(id);
    if (it == customerIndex.end() || it->second >= static_cast<int>(customers.size())) {
        return nullptr;
    }
    return &customers[it->second];
}

const Order* GameState::findOrder(const int id) const {
    const auto it = orderIndex.find(id);
    if (it == orderIndex.end() || it->second >= static_cast<int>(orders.size())) {
        return nullptr;
    }
    return &orders[it->second];
}

const Deliver* GameState::findDeliver(const int id) const {
    const auto it = deliverIndex.find(id);
    if (it == deliverIndex.end() || it->second >= static_cast<int>(delivers.size())) {
        return nullptr;
    }
    return &delivers[it->second];
}

const Warehouse* GameState::findWarehouse(const int id) const {
    const auto it = warehouseIndex.find(id);
    if (it == warehouseIndex.end() || it->second >= static_cast<int>(warehouses.size())) {
        return nullptr;
    }
    return &warehouses[it->second];
}

const Merchant* GameState::findMerchant(const int id) const {
    const auto it = merchantIndex.find(id);
    if (it == merchantIndex.end() || it->second >= static_cast<int>(merchants.size())) {
        return nullptr;
    }
    return &merchants[it->second];
}

const Customer* GameState::findCustomer(const int id) const {
    const auto it = customerIndex.find(id);
    if (it == customerIndex.end() || it->second >= static_cast<int>(customers.size())) {
        return nullptr;
    }
    return &customers[it->second];
}

// ========== 辅助 ==========
std::vector<int> GameState::getIdleDelivers() const {
    std::vector<int> result;
    for (const auto& d : delivers) {
        if (d.status() == DeliverStatus::IDLE) {
            result.push_back(d.id());
        }
    }
    return result;
}

std::vector<int> GameState::getPendingOrdersByMerchant(const int merchantId) const {
    std::vector<int> result;
    for (int orderId : pendingOrderIds) {
        if (const auto* order = findOrder(orderId); order && order->merchantId() == merchantId) {
            result.push_back(orderId);
        }
    }
    return result;
}

// ========== 内部方法 ==========
void GameState::rebuildIndex() {
    orderIndex.clear();
    for (size_t i = 0; i < orders.size(); ++i) {
        orderIndex[orders[i].id()] = static_cast<int>(i);
    }

    deliverIndex.clear();
    for (size_t i = 0; i < delivers.size(); ++i) {
        deliverIndex[delivers[i].id()] = static_cast<int>(i);
    }

    warehouseIndex.clear();
    for (size_t i = 0; i < warehouses.size(); ++i) {
        warehouseIndex[warehouses[i].id()] = static_cast<int>(i);
    }

    merchantIndex.clear();
    for (size_t i = 0; i < merchants.size(); ++i) {
        merchantIndex[merchants[i].id()] = static_cast<int>(i);
    }

    customerIndex.clear();
    for (size_t i = 0; i < customers.size(); ++i) {
        customerIndex[customers[i].id()] = static_cast<int>(i);
    }
}

void GameState::initDefaultCity() {
    // 从配置文件加载，如果失败则使用默认值
    auto config = utils::loadJSON("resource/config/city_config.json");
    if (config.empty()) {
        LOG_WARN("城市配置文件未找到，使用默认城市");

        // 20x20 网格
        cityGrid = CityGrid(20);

        // 默认商家
        merchants.clear();
        merchants.emplace_back(0, "星巴克", 3, 5, MerchantType::CAFE);
        merchants.emplace_back(1, "优衣库", 12, 8, MerchantType::RETAIL);
        merchants.emplace_back(2, "海底捞", 8, 15, MerchantType::RESTAURANT);

        // 默认客户
        customers.clear();
        customers.emplace_back(0, "阳光写字楼", 15, 16, 2);
        customers.emplace_back(1, "幸福小区", 7, 14, 0);
        customers.emplace_back(2, "万达广场", 4, 18, 1);

        // 默认仓库
        warehouses.clear();
        warehouses.emplace_back(0, "城东转运中心", 10, 10, 500);

        // 默认骑手
        delivers.clear();
        delivers.emplace_back(0, "小王", 10, 9, VehicleType::BICYCLE);
        delivers.emplace_back(1, "小李", 11, 10, VehicleType::VAN);

        // 默认车辆
        vehicles.clear();
        vehicles.push_back(Vehicle::createBicycle(0));
        vehicles.push_back(Vehicle::createVan(1));
        vehicles.push_back(Vehicle::createTruck(2));

        rebuildIndex();
        return;
    }

    // 从配置文件加载
    try {
        const int size = config.value("grid_size", 20);
        cityGrid = CityGrid(size);

        // 加载障碍物
        if (config.contains("obstacles")) {
            for (const auto& obs : config["obstacles"]) {
                const int x = obs.value("x", 0);
                const int y = obs.value("y", 0);
                cityGrid.addObstacle(x, y);
            }
        }

        // 加载商家
        merchants.clear();
        if (config.contains("merchants")) {
            for (const auto& m : config["merchants"]) {
                int id = m.value("id", static_cast<int>(merchants.size()));
                std::string name = m.value("name", "商家" + std::to_string(id));
                int x = m.value("x", 0);
                int y = m.value("y", 0);
                int type = m.value("type", 1);
                merchants.emplace_back(id, name, x, y, static_cast<MerchantType>(type));
            }
        }

        // 加载客户
        customers.clear();
        if (config.contains("customers")) {
            for (const auto& c : config["customers"]) {
                int id = c.value("id", static_cast<int>(customers.size()));
                std::string name = c.value("name", "客户" + std::to_string(id));
                int x = c.value("x", 0);
                int y = c.value("y", 0);
                int vip = c.value("vip_level", 0);
                customers.emplace_back(id, name, x, y, vip);
            }
        }

        // 加载仓库
        warehouses.clear();
        if (config.contains("warehouses")) {
            for (const auto& w : config["warehouses"]) {
                int id = w.value("id", static_cast<int>(warehouses.size()));
                std::string name = w.value("name", "仓库" + std::to_string(id));
                int x = w.value("x", 0);
                int y = w.value("y", 0);
                int cap = w.value("capacity", 500);
                warehouses.emplace_back(id, name, x, y, cap);
            }
        }

        rebuildIndex();
        LOG_INFO("城市配置加载成功");

    } catch (const std::exception& e) {
        LOG_ERROR("解析城市配置失败: " + std::string(e.what()));
        initDefaultCity();  // 回退
    }
}

} // namespace delivery