#pragma once

#include "../utils/json_.h"

#include <QString>
#include <vector>
#include <functional>

namespace systems {

    /// 等级配置
    struct LevelConfig {
        int level = 0;
        QString name;
        int minReputation = 0;
        int upgradeCost = 0;
        int maxDelivers = 5;
        int unlockWarehouse = 1;
    };

    /**
     * @brief 公司升级系统
     */
    class UpgradeSystem {
    public:
        UpgradeSystem() = default;

        void loadFromJSON(const utils::json& config);

        /// 尝试升级，成功返回 true
        bool tryUpgrade();

        /// 增加/扣除资金，返回是否成功
        bool addMoney(double amount);

        /// 增加/扣减声望
        void addReputation(double amount);

        // ── 查询 ──
        [[nodiscard]] int getLevel() const { return m_level; }
        [[nodiscard]] double getMoney() const { return m_money; }
        [[nodiscard]] double getReputation() const { return m_reputation; }
        [[nodiscard]] const LevelConfig& getCurrentLevelConfig() const { return m_levels[m_level]; }
        [[nodiscard]] const LevelConfig& getNextLevelConfig() const;
        [[nodiscard]] bool canUpgrade() const;
        [[nodiscard]] int getMaxDelivers() const { return getCurrentLevelConfig().maxDelivers; }

        // 信号回调：升级成功
        std::function<void(int newLevel, const QString& newName)> onLevelUp;

    private:
        double m_money = 10000.0;
        double m_reputation = 60.0;
        int m_level = 0;
        std::vector<LevelConfig> m_levels;
    };

}
