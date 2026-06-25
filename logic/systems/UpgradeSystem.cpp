#include "UpgradeSystem.h"
#include "../utils/log_.h"
#include "../utils/random_.h"

using namespace delivery::utils;

namespace delivery {

// ========== 尝试升级 ==========
bool UpgradeSystem::tryUpgrade() {
    auto& state = GameState::instance();

    if (!canUpgrade()) {
        LOG_DEBUG("升级条件不满足");
        return false;
    }

    const auto* next = nextLevelInfo();
    if (!next) return false;

    // 扣费
    if (!state.spendMoney(next->upgradeCost)) {
        return false;
    }

    // 执行升级
    state.doUpgrade();
    applyUpgradeBenefits();

    LOG_INFO("🎉 公司升级！当前等级: " + state.currentLevelInfo().name);
    return true;
}

// ========== 检查是否可升级 ==========
bool UpgradeSystem::canUpgrade() const {
    const auto& state = GameState::instance();
    return state.canUpgrade();
}

// ========== 获取下一级信息 ==========
const LevelInfo* UpgradeSystem::nextLevelInfo() const {
    const auto& state = GameState::instance();
    const int idx = state.levelIndex();

    if (idx >= static_cast<int>(GameState::LEVEL_CONFIG.size()) - 1) {
        return nullptr;  // 已是最高级
    }

    return &GameState::LEVEL_CONFIG[idx + 1];
}

// ========== 应用升级奖励 ==========
void UpgradeSystem::applyUpgradeBenefits() {
    auto& state = GameState::instance();

    // 根据新等级解锁功能
    switch (state.level()) {
        case CompanyLevel::LOCAL_DEPOT:
            LOG_INFO("🔓 解锁：第2个仓库槽位");
            // 实际在UI中显示
            break;

        case CompanyLevel::CITY_NETWORK:
            LOG_INFO("🔓 解锁：面包车（VAN）");
            // 添加面包车到车辆列表
            if (state.vehicles.size() < 3) {
                state.vehicles.push_back(Vehicle::createVan(
                    static_cast<int>(state.vehicles.size())
                ));
            }
            break;

        case CompanyLevel::REGIONAL_HUB:
            LOG_INFO("🔓 解锁：冷链货车（TRUCK）");
            if (state.vehicles.size() < 4) {
                state.vehicles.push_back(Vehicle::createTruck(
                    static_cast<int>(state.vehicles.size())
                ));
            }
            break;

        case CompanyLevel::METRO_CHAIN:
            LOG_INFO("🔓 解锁：全城覆盖！");
            break;

        default:
            break;
    }

    // 升级奖励：免费获得一个骑手
    if (state.delivers.size() < static_cast<size_t>(state.currentLevelInfo().maxDelivers)) {
        int newId = static_cast<int>(state.delivers.size());
        std::string name = "骑手" + std::to_string(newId + 1);
        // 随机位置
        int x = 5 + randomInt(0, 10);
        int y = 5 + randomInt(0, 10);
        state.delivers.emplace_back(newId, name, x, y, VehicleType::BICYCLE);
        state.deliverIndex[newId] = static_cast<int>(state.delivers.size()) - 1;
        LOG_INFO("🎁 新骑手入职：" + name);
    }
}

// ========== 解锁新功能 ==========
void UpgradeSystem::unlockNewFeatures(CompanyLevel newLevel) {
    // 实际功能已在 applyUpgradeBenefits 中实现
    // 此方法留作扩展
}

} // namespace delivery