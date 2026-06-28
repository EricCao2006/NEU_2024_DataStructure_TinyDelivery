#include "UpgradeSystem.h"
#include "../utils/log_.h"

namespace systems {

    void UpgradeSystem::loadFromJSON(const utils::json& config) {
        m_money = config.value("initial_money", 10000.0);
        m_reputation = config.value("initial_reputation", 60.0);
        m_level = config.value("initial_level", 0);

        m_levels.clear();
        if (config.contains("level_config") && config["level_config"].is_array()) {
            for (const auto& j : config["level_config"]) {
                LevelConfig lc;
                lc.level = j.value("level", 0);
                lc.name = QString::fromStdString(j.value("name", ""));
                lc.minReputation = j.value("min_reputation", 0);
                lc.upgradeCost = j.value("upgrade_cost", 0);
                lc.maxDelivers = j.value("max_delivers", 5);
                lc.unlockWarehouse = j.value("unlock_warehouse", 1);
                m_levels.push_back(lc);
            }
        }

        utils::Logger::log(1, std::string("UpgradeSystem loaded: level=") + std::to_string(m_level) +
                           " money=" + std::to_string(m_money));
    }

    bool UpgradeSystem::tryUpgrade() {
        if (!canUpgrade()) return false;

        const auto& next = getNextLevelConfig();
        m_money -= next.upgradeCost;
        m_level++;

        if (onLevelUp) {
            onLevelUp(m_level, getCurrentLevelConfig().name);
        }

        utils::Logger::log(1, std::string("Upgraded to level ") + std::to_string(m_level) +
                           ": " + getCurrentLevelConfig().name.toStdString());
        return true;
    }

    bool UpgradeSystem::addMoney(const double amount) {
        if (m_money + amount < 0) return false;
        m_money += amount;
        return true;
    }

    void UpgradeSystem::addReputation(const double amount) {
        m_reputation = std::max(0.0, std::min(100.0, m_reputation + amount));
    }

    const LevelConfig& UpgradeSystem::getNextLevelConfig() const {
        static LevelConfig dummy;
        if (m_level + 1 >= static_cast<int>(m_levels.size())) return dummy;
        return m_levels[m_level + 1];
    }

    bool UpgradeSystem::canUpgrade() const {
        if (m_level + 1 >= static_cast<int>(m_levels.size())) return false;
        const auto& next = m_levels[m_level + 1];
        return m_money >= next.upgradeCost && m_reputation >= next.minReputation;
    }

}
