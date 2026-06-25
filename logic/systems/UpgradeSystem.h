#pragma once
/**
 * @file UpgradeSystem.h
 * @brief 公司升级系统
 */

#include "../GameState.h"

namespace delivery {

    /**
     * @brief 升级系统
     */
    class UpgradeSystem {
    public:
        UpgradeSystem() = default;
        ~UpgradeSystem() = default;

        /**
         * @brief 尝试升级公司
         * @return true 升级成功
         */
        bool tryUpgrade();

        /**
         * @brief 检查是否可升级
         */
        [[nodiscard]] bool canUpgrade() const;

        /**
         * @brief 获取下一级信息
         */
        [[nodiscard]] const LevelInfo* nextLevelInfo() const;

        /**
         * @brief 升级后解锁内容
         */
        void applyUpgradeBenefits();

    private:
        void unlockNewFeatures(CompanyLevel newLevel);
    };

} // namespace delivery