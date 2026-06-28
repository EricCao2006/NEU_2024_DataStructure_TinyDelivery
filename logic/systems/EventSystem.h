#pragma once

#include "../utils/json_.h"

#include <QString>
#include <vector>
#include <functional>

namespace systems {

    /// 单个事件定义
    struct GameEvent {
        int type = 0;
        QString title;
        QString description;
        int durationMin = 0;   // 最短持续时间（秒）
        int durationMax = 0;   // 最长持续时间（秒）
        int impact = 0;        // 影响值（正=正面，负=负面）
    };

    /// 正在生效的事件
    struct ActiveEvent {
        GameEvent event;
        int remainingSeconds = 0;
    };

    /**
     * @brief 随机事件系统
     */
    class EventSystem {
    public:
        EventSystem() = default;

        /// 从 JSON 加载事件配置
        void loadFromJSON(const utils::json& config);

        /// 每帧更新：倒计时 + 随机触发
        void tick(double deltaSeconds);

        /// 获取当前生效中的事件
        [[nodiscard]] const std::vector<ActiveEvent>& getActiveEvents() const { return m_activeEvents; }

        /// 获取综合影响值（所有活跃事件 impact 之和）
        [[nodiscard]] int getTotalImpact() const;

        /// 设置触发间隔（秒）
        void setTriggerInterval(int seconds) { m_triggerInterval = seconds; }

        // 信号回调：新事件触发
        std::function<void(const GameEvent&)> onEventTriggered;
        // 信号回调：事件结束
        std::function<void(const GameEvent&)> onEventEnded;

    private:
        std::vector<GameEvent> m_eventPool;
        std::vector<ActiveEvent> m_activeEvents;
        int m_triggerInterval = 60;  // 默认60秒触发一次
        double m_timer = 0.0;

        void triggerRandomEvent();
    };

}
