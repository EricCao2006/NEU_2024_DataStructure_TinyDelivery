#include "EventSystem.h"
#include "../utils/log_.h"
#include "../utils/random_.h"

namespace systems {

    void EventSystem::loadFromJSON(const utils::json& config) {
        m_eventPool.clear();
        m_triggerInterval = config.value("trigger_interval", 60);

        if (config.contains("events") && config["events"].is_array()) {
            for (const auto& j : config["events"]) {
                GameEvent e;
                e.type = j.value("type", 0);
                e.title = QString::fromStdString(j.value("title", ""));
                e.description = QString::fromStdString(j.value("description", ""));
                if (j.contains("duration_range") && j["duration_range"].is_array()) {
                    e.durationMin = j["duration_range"][0].get<int>();
                    e.durationMax = j["duration_range"][1].get<int>();
                }
                e.impact = j.value("impact", 0);
                m_eventPool.push_back(e);
            }
        }

        utils::Logger::log(1, "EventSystem loaded: " +
                           std::to_string(m_eventPool.size()) + " events");
    }

    void EventSystem::tick(const double deltaSeconds) {
        // 更新活跃事件倒计时
        for (auto it = m_activeEvents.begin(); it != m_activeEvents.end(); ) {
            it->remainingSeconds -= static_cast<int>(deltaSeconds);
            if (it->remainingSeconds <= 0) {
                if (onEventEnded) onEventEnded(it->event);
                it = m_activeEvents.erase(it);
            } else {
                ++it;
            }
        }

        // 随机触发新事件
        m_timer += deltaSeconds;
        if (m_timer >= m_triggerInterval) {
            m_timer = 0.0;
            triggerRandomEvent();
        }
    }

    int EventSystem::getTotalImpact() const {
        int total = 0;
        for (const auto&[event, remainingSeconds] : m_activeEvents) {
            total += event.impact;
        }
        return total;
    }

    void EventSystem::triggerRandomEvent() {
        if (m_eventPool.empty()) return;

        const int idx = utils::randomInt(0, static_cast<int>(m_eventPool.size()) - 1);
        const auto& evt = m_eventPool[idx];

        // 瞬时事件（duration_range=[0,0]）立即触发并消失
        if (evt.durationMin == 0 && evt.durationMax == 0) {
            if (onEventTriggered) onEventTriggered(evt);
            return;
        }

        const int duration = utils::randomInt(evt.durationMin, evt.durationMax);
        ActiveEvent ae;
        ae.event = evt;
        ae.remainingSeconds = duration;
        m_activeEvents.push_back(ae);

        if (onEventTriggered) onEventTriggered(evt);

        utils::Logger::log(1, "Event triggered: " + evt.title.toStdString() +
                           " for " + std::to_string(duration) + "s");
    }

}
