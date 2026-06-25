#include "EventSystem.h"
#include "../utils/log_.h"
#include "../utils/random_.h"

namespace delivery {

// ========== 检查事件 ==========
void EventSystem::checkEvents() {
    const auto& state = GameState::instance();

    // 检查当前活跃事件是否过期
    if (m_activeEvent && m_activeEvent->active) {
        m_activeEvent->duration -= 1;  // 每秒减少
        if (m_activeEvent->duration <= 0) {
            clearEvent();
        }
        return;
    }

    // 随机触发新事件
    m_timer += 1.0;
    const double interval = m_eventInterval * (0.5 + 0.5 * utils::random01());

    if (m_timer >= interval) {
        m_timer = 0.0;

        // 只有信誉在 20-80 之间时才触发事件（太差或太好都少事件）
        if (state.reputation() > 20 && state.reputation() < 80) {
            const auto event = generateEvent();
            if (event.type != EventType::RAIN_STORM) {  // 非空事件
                applyEvent(event);
                if (m_callback) {
                    m_callback(event);
                }
                LOG_INFO("🎯 触发事件: " + event.title);
            }
        }
    }
}

// ========== 应用事件 ==========
void EventSystem::applyEvent(const GameEvent& event) {
    auto& state = GameState::instance();

    // 如果已有活跃事件，先清除
    if (m_activeEvent) {
        clearEvent();
    }

    m_activeEvent = std::make_unique<GameEvent>(event);
    m_activeEvent->active = true;

    // 应用事件效果
    switch (event.type) {
        case EventType::RAIN_STORM:
            LOG_INFO("🌧️ 暴雨：所有骑手速度减半");
            // 实际效果在 Deliver::speedFactor 中体现
            break;

        case EventType::DOUBLE_ELEVEN:
            LOG_INFO("📦 双十一爆单：订单生成速度 ×3");
            // 效果在 GameEngine::generateOrders 中体现
            break;

        case EventType::VIP_COMPLAINT:
            state.changeReputation(-10);
            LOG_WARN("👎 VIP投诉，信誉 -10");
            break;

        case EventType::FLEET_EXPANSION:
            LOG_INFO("🚗 车队扩张机会！");
            // 给玩家一个便宜买车的机会（UI中处理）
            break;

        case EventType::FUEL_PRICE_SURGE:
            LOG_WARN("⛽ 油价上涨，运营成本 +20%");
            // 影响利润计算
            break;

        case EventType::HEAT_WAVE:
            LOG_INFO("☀️ 高温：骑手疲劳速度 +50%");
            break;

        default:
            break;
    }
}

// ========== 清除事件 ==========
void EventSystem::clearEvent() {
    if (m_activeEvent) {
        LOG_INFO("事件结束: " + m_activeEvent->title);
        m_activeEvent->active = false;
        m_activeEvent.reset();
    }
}

// ========== 生成随机事件 ==========
GameEvent EventSystem::generateEvent() {
    GameEvent event;
    event.active = true;
    event.duration = 60 + utils::randomInt(0, 120);  // 1-3分钟

    int type = utils::randomInt(0, 5);
    event.type = static_cast<EventType>(type);

    switch (event.type) {
        case EventType::RAIN_STORM:
            event.title = "暴雨红色预警";
            event.description = "全市暴雨，骑手配送速度下降50%";
            event.impact = -50;
            break;

        case EventType::HEAT_WAVE:
            event.title = "高温橙色预警";
            event.description = "气温超过38℃，骑手易疲劳";
            event.impact = -30;
            break;

        case EventType::DOUBLE_ELEVEN:
            event.title = "双十一购物节";
            event.description = "订单量暴涨！抓住机会！";
            event.impact = 100;
            break;

        case EventType::VIP_COMPLAINT:
            event.title = "VIP客户投诉";
            event.description = "配送延误，客户表示不满";
            event.impact = -10;
            break;

        case EventType::FLEET_EXPANSION:
            event.title = "车队优惠促销";
            event.description = "新车采购享8折优惠！";
            event.impact = 20;
            break;

        case EventType::FUEL_PRICE_SURGE:
            event.title = "油价上涨";
            event.description = "燃油成本增加20%";
            event.impact = -20;
            break;

        default:
            event.title = "未知事件";
            event.description = "发生了一些事情...";
            event.impact = 0;
            break;
    }

    return event;
}

// ========== 设置回调 ==========
void EventSystem::setEventCallback(std::function<void(const GameEvent&)> callback) {
    m_callback = std::move(callback);
}

} // namespace delivery