#include "ContractSystem.h"
#include "../utils/log_.h"
#include "../utils/random_.h"

namespace delivery {

// ========== 更新 ==========
void ContractSystem::update() {
    auto& state = GameState::instance();

    for (auto& contract : m_contracts) {
        if (contract.status != ContractStatus::ACTIVE) continue;

        // 检查是否到期
        if (const int elapsed = state.gameTime() - contract.startTime; elapsed >= contract.duration) {
            contract.status = ContractStatus::EXPIRED;
            LOG_INFO("客户 #" + std::to_string(contract.customerId) + " 合同已到期");

            // 影响信誉
            state.changeReputation(-2);
        }
    }
}

// ========== 获取合同 ==========
Contract* ContractSystem::getContract(const int customerId) {
    for (auto& contract : m_contracts) {
        if (contract.customerId == customerId) {
            return &contract;
        }
    }
    return nullptr;
}

// ========== 续约 ==========
bool ContractSystem::renewContract(const int customerId) {
    auto& state = GameState::instance();
    auto* contract = getContract(customerId);

    if (!contract) {
        // 新客户，创建合同
        Contract newContract;
        newContract.customerId = customerId;
        newContract.status = ContractStatus::ACTIVE;
        newContract.startTime = state.gameTime();
        newContract.duration = 3600;

        int cost = calculateRenewalCost(customerId);
        if (!state.spendMoney(cost)) {
            LOG_WARN("续约失败：资金不足");
            return false;
        }

        m_contracts.push_back(newContract);
        LOG_INFO("新客户 #" + std::to_string(customerId) + " 签约成功");
        return true;
    }

    if (contract->status == ContractStatus::ACTIVE) {
        LOG_DEBUG("客户 #" + std::to_string(customerId) + " 合同仍在有效期内");
        return true;
    }

    // 续约过期合同
    if (const int cost = calculateRenewalCost(customerId); !state.spendMoney(cost)) {
        LOG_WARN("续约失败：资金不足");
        return false;
    }

    contract->status = ContractStatus::ACTIVE;
    contract->startTime = state.gameTime();
    contract->duration = 3600 + utils::randomInt(0, 1800);  // 1-1.5小时

    LOG_INFO("客户 #" + std::to_string(customerId) + " 续约成功");
    return true;
}

// ========== 计算续约费用 ==========
int ContractSystem::calculateRenewalCost(const int customerId) const {
    auto& state = GameState::instance();
    auto* customer = state.findCustomer(customerId);

    if (!customer) return 500;

    // 基础费用 + VIP加成 + 信誉调整
    constexpr int base = 500;
    const int vipBonus = customer->vipLevel() * 200;
    const int reputationDiscount = (100 - state.reputation()) / 10;

    return base + vipBonus + reputationDiscount;
}

} // namespace delivery