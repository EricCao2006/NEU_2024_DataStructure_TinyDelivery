#include "WarehouseSystem.h"
#include "../utils/log_.h"
#include "../GameState.h"

namespace delivery {

// ========== 更新 ==========
void WarehouseSystem::update(const double deltaSeconds) {
    m_timer += deltaSeconds;

    if (m_timer >= UPDATE_INTERVAL) {
        m_timer = 0.0;
        checkWarehouseStatus();
    }
}

// ========== 收货 ==========
bool WarehouseSystem::receiveGoods(int warehouseId, int count) {
    auto& state = GameState::instance();
    auto* warehouse = state.findWarehouse(warehouseId);

    if (!warehouse) {
        LOG_WARN("仓库 #" + std::to_string(warehouseId) + " 不存在");
        return false;
    }

    bool success = warehouse->receive(count);
    if (!success) {
        LOG_WARN("仓库 " + warehouse->name() + " 已爆仓！");
    }
    return success;
}

// ========== 出货 ==========
int WarehouseSystem::dispatchGoods(int warehouseId, int count) {
    auto& state = GameState::instance();
    auto* warehouse = state.findWarehouse(warehouseId);

    if (!warehouse) {
        LOG_WARN("仓库 #" + std::to_string(warehouseId) + " 不存在");
        return 0;
    }

    return warehouse->dispatch(count);
}

// ========== 检查仓库状态 ==========
void WarehouseSystem::checkWarehouseStatus() {
    for (auto& state = GameState::instance(); auto& warehouse : state.warehouses) {
        const double loadRate = warehouse.loadRate();

        if (loadRate > 0.9) {
            LOG_WARN("仓库 " + warehouse.name() +
                     " 即将爆仓！负载率: " + std::to_string(loadRate * 100) + "%");
        } else if (loadRate > 0.7) {
            LOG_DEBUG("仓库 " + warehouse.name() +
                      " 负载率: " + std::to_string(loadRate * 100) + "%");
        }

        // 自动处理：如果爆仓，尝试扩容
        if (loadRate >= 1.0) {
            // 检查是否有钱升级
            if (state.money() >= warehouse.upgradeCost()) {
                warehouse.upgrade();
                LOG_INFO("仓库 " + warehouse.name() +
                         " 已自动升级到 Lv." + std::to_string(warehouse.level()));
                state.spendMoney(warehouse.upgradeCost());
            }
        }
    }
}

} // namespace delivery