#pragma once
/**
 * @file EventSystem.h
 * @brief 随机事件系统
 */

#include "../GameState.h"
#include <functional>
#include <string>

namespace delivery {

    /**
     * @brief 事件类型
     */
    enum class EventType : uint8_t {
        RAIN_STORM,        ///< 暴雨
        HEAT_WAVE,         ///< 高温
        DOUBLE_ELEVEN,     ///< 双十一爆单
        VIP_COMPLAINT,     ///< VIP投诉
        FLEET_EXPANSION,   ///< 车队扩张机会
        FUEL_PRICE_SURGE   ///< 油价上涨
    };

    /**
     * @brief 事件信息
     */
    struct GameEvent {
        EventType type;
        std::string title;
        std::string description;
        int duration = 300;            ///< 持续时长（秒）
        int impact = 0;                ///< 影响值
        bool active = false;
    };

    class EventSystem {
    public:
        EventSystem() = default;
        ~EventSystem() = default;

        /**
         * @brief 检查并触发事件
         */
        void checkEvents();

        /**
         * @brief 应用事件效果
         */
        void applyEvent(const GameEvent& event);

        /**
         * @brief 清除事件
         */
        void clearEvent();

        /**
         * @brief 获取当前活跃事件
         */
        [[nodiscard]] const GameEvent* currentEvent() const { return m_activeEvent ? &*m_activeEvent : nullptr; }

        /**
         * @brief 设置事件回调
         */
        void setEventCallback(std::function<void(const GameEvent&)> callback);

    private:
        /**
         * @brief 随机生成事件
         */
        GameEvent generateEvent();

        std::unique_ptr<GameEvent> m_activeEvent;
        double m_timer = 0.0;
        double m_eventInterval = 60.0;   ///< 平均每60秒触发一次
        std::function<void(const GameEvent&)> m_callback;
    };

} // namespace delivery