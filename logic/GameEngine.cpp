#include "GameEngine.h"
#include "utils/log_.h"
#include "utils/random_.h"
#include <thread>
#include <chrono>
#include <memory>

namespace delivery {

// ========== 单例 ==========
GameEngine& GameEngine::instance() {
    static GameEngine engine;
    return engine;
}

// ========== 初始化 ==========
    bool GameEngine::initialize(const bool loadSave) {
    LOG_INFO("游戏引擎初始化...");

    auto& state = GameState::instance();

    if (loadSave) {
        state.loadFromFile("resource/save/save_000.json");
    } else {
        state.reset();
    }

    // 初始化子系统
    m_dispatchSystem = std::make_unique<DispatchSystem>();
    m_warehouseSystem = std::make_unique<WarehouseSystem>();
    m_contractSystem = std::make_unique<ContractSystem>();
    m_upgradeSystem = std::make_unique<UpgradeSystem>();
    m_eventSystem = std::make_unique<EventSystem>();

    // ★ 连接 EventSystem 的事件到引擎
    m_eventSystem->setEventCallback([this](const GameEvent& event) {
        if (m_eventCallback) {
            m_eventCallback(event.title, event.description);
        }
        LOG_INFO("事件触发: " + event.title);
    });

    m_orderGenerationTimer = 0.0;
    m_accumulator = 0.0;
    m_state = EngineState::RUNNING;

    LOG_INFO("游戏引擎初始化完成");
    return true;
}
// ========== 运行（阻塞） ==========
void GameEngine::run() {
    if (m_state == EngineState::STOPPED) {
        initialize(false);
    }

    LOG_INFO("游戏主循环启动");

    auto lastTime = utils::now();

    while (m_state != EngineState::STOPPED) {
        auto currentTime = utils::now();
        auto deltaMs = std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
        lastTime = currentTime;

        // 限制最大增量（防止调试断点后跳帧太狠）
        if (deltaMs > 100.0) deltaMs = 100.0;

        update(deltaMs);

        // 帧率控制（60fps）
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    LOG_INFO("游戏主循环结束");
}

// ========== 更新 ==========
void GameEngine::update(double deltaMs) {
    if (m_state == EngineState::STOPPED) return;

    // 处理暂停
    if (m_state == EngineState::PAUSED) {
        if (m_frameCallback) m_frameCallback();
        return;
    }

    // 将真实时间转换为游戏时间
    const double gameDeltaSeconds = m_gameTimer.tick(deltaMs);

    if (gameDeltaSeconds <= 0) {
        if (m_frameCallback) m_frameCallback();
        return;
    }

    // 固定时间步长累积
    m_accumulator += gameDeltaSeconds;

    while (m_accumulator >= FIXED_TIME_STEP) {
        tick(FIXED_TIME_STEP);
        m_accumulator -= FIXED_TIME_STEP;
    }

    // 帧回调
    if (m_frameCallback) m_frameCallback();
}

// ========== 主逻辑 ==========
    void GameEngine::tick(const double gameDeltaSeconds) {
    auto& state = GameState::instance();

    // 1. 推进游戏时间
    state.advanceTime(static_cast<int>(gameDeltaSeconds));

    // 2. 生成订单
    m_orderGenerationTimer += gameDeltaSeconds;
    if (m_orderGenerationTimer >= ORDER_GENERATE_INTERVAL) {
        m_orderGenerationTimer -= ORDER_GENERATE_INTERVAL;
        generateOrders();
    }

    // 3. 调度系统 - 分配订单给骑手
    if (m_dispatchSystem) {
        m_dispatchSystem->processOrders();
    }

    // 4. 更新骑手位置
    updateDelivers(gameDeltaSeconds);

    // 5. 检查订单完成
    checkCompletedOrders();

    // 6. 仓库系统 - 处理吞吐
    if (m_warehouseSystem) {
        m_warehouseSystem->update(gameDeltaSeconds);
    }

    // 7. ★ 事件系统 - 检查触发
    if (m_eventSystem) {
        m_eventSystem->checkEvents();
    }

    // 8. 合同系统 - 检查续约
    if (m_contractSystem) {
        m_contractSystem->update();
    }

    // 9. 公司升级检查（自动或提示）
    // 升级在UI中由玩家触发，Engine只做条件检查
}

// ========== 生成订单 ==========
void GameEngine::generateOrders() {
    auto& state = GameState::instance();

    // 从活跃的商家中随机选一个
    std::vector<int> activeMerchants;
    for (const auto& m : state.merchants) {
        if (m.isActive()) {
            activeMerchants.push_back(m.id());
        }
    }

    if (activeMerchants.empty()) return;

    // 随机选商家
    int merchantId = activeMerchants[utils::randomInt(0, static_cast<int>(activeMerchants.size()) - 1)];
    auto* merchant = state.findMerchant(merchantId);
    if (!merchant) return;

    // 选一个客户（随机）
    if (state.customers.empty()) return;
    int customerIdx = utils::randomInt(0, static_cast<int>(state.customers.size()) - 1);
    int customerId = state.customers[customerIdx].id();

    // 创建订单
    int orderId = static_cast<int>(state.orders.size());
    double weight = merchant->averageWeight() * (0.8 + 0.4 * utils::random01());

    Order order(orderId, merchantId, customerId, weight);
    order.setCreateTime(state.gameTime());

    // VIP客户优先级高
    auto* customer = state.findCustomer(customerId);
    if (customer && customer->vipLevel() > 0) {
        order.setPriority(customer->vipLevel());
    }

    state.orders.push_back(order);
    state.pendingOrderIds.push_back(orderId);
    merchant->addOrder();

    // 更新索引
    state.orderIndex[orderId] = static_cast<int>(state.orders.size()) - 1;

    LOG_DEBUG("生成订单 #" + std::to_string(orderId) +
              " 商家:" + std::to_string(merchantId) +
              " -> 客户:" + std::to_string(customerId) +
              " 重量:" + std::to_string(weight));
}

// ========== 更新骑手 ==========
void GameEngine::updateDelivers(double deltaSeconds) {
    for (auto& state = GameState::instance(); auto& deliver : state.delivers) {
        // 只更新有订单的骑手
        if (deliver.currentOrderId() < 0) continue;

        auto* order = state.findOrder(deliver.currentOrderId());
        if (!order) continue;

        // 获取目标位置
        const auto [fst, snd] = order->currentPosition();
        if (fst < 0) continue;

        // 移动

        if (bool arrived = deliver.moveToward(fst, snd)) {
            // 前进到路径下一节点
            if (bool completed = order->advance()) {
                // 订单完成
                LOG_DEBUG("订单 #" + std::to_string(order->id()) + " 派送完成");
                deliver.setStatus(DeliverStatus::IDLE);
                deliver.setCurrentOrderId(-1);
                // record 在 checkCompletedOrders 中处理
            }
        }
    }
}

// ========== 检查订单完成 ==========
void GameEngine::checkCompletedOrders() {
    for (auto& state = GameState::instance(); auto& order : state.orders) {
        if (order.status() != OrderStatus::COMPLETED) continue;

        int orderId = order.id();
        if (orderId < 0) continue;

        // ★ 计算实际响应时间：当前时间 - 创建时间
        const auto responseTime = static_cast<double>(state.gameTime() - order.createTime());

        const double revenue = 10.0 + order.weight() * 2.0;
        state.recordOrderCompleted(revenue);

        if (auto it = std::ranges::find(state.pendingOrderIds, orderId); it != state.pendingOrderIds.end()) {
            state.pendingOrderIds.erase(it);
        }

        if (auto* customer = state.findCustomer(order.customerId())) {
            const bool onTime = responseTime < customer->maxWaitTime();
            customer->recordDelivery(responseTime, onTime);
        }

        LOG_DEBUG("订单 #" + std::to_string(orderId) +
                  " 已完成，响应时间: " + std::to_string(responseTime) + "s" +
                  " 收入: " + std::to_string(revenue));
    }
}
    
// ========== 控制 ==========
void GameEngine::pause() {
    if (m_state == EngineState::RUNNING) {
        m_state = EngineState::PAUSED;
        LOG_INFO("游戏暂停");
    }
}

void GameEngine::resume() {
    if (m_state == EngineState::PAUSED) {
        m_state = EngineState::RUNNING;
        LOG_INFO("游戏继续");
    }
}

void GameEngine::togglePause() {
    if (m_state == EngineState::RUNNING) {
        pause();
    } else if (m_state == EngineState::PAUSED) {
        resume();
    }
}

void GameEngine::setSpeed(const double speed) {
    m_gameTimer.setSpeed(speed);
    LOG_DEBUG("游戏速度: " + std::to_string(speed));
}

void GameEngine::stop() {
    m_state = EngineState::STOPPED;
    LOG_INFO("游戏引擎停止");
}

// ========== 回调 ==========
void GameEngine::setFrameCallback(std::function<void()> callback) {
    m_frameCallback = std::move(callback);
}

void GameEngine::setEventCallback(std::function<void(const std::string&, const std::string&)> callback) {
    m_eventCallback = std::move(callback);
}

} // namespace delivery