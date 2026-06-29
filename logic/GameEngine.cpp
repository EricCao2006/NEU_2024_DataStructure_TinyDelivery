#include "GameEngine.h"

#include "GameState.h"
#include "utils/log_.h"
#include "utils/random_.h"

GameEngine::GameEngine(QObject* parent) : QObject(parent) {}

void GameEngine::init(const QString& configDir) {
    utils::Logger::log(0, "[GameEngine] Loading configs from: " + configDir.toStdString());
    loadConfigs(configDir);
    utils::Logger::log(0, "[GameEngine] Configs loaded, initializing dispatch...");
    m_dispatch.init(m_grid);

    // 连接信号
    m_dispatch.onOrderCompleted = [this](const models::Order& order) {
        m_upgrade.addMoney(order.getDeliveryFee());
        m_upgrade.addReputation(3.0);  // 每单+3声望
        emit orderCompleted(order.getId());
        emit moneyChanged(m_upgrade.getMoney());
        emit reputationChanged(m_upgrade.getReputation());
    };

    m_eventSystem.onEventTriggered = [this](const systems::GameEvent& evt) {
        emit eventTriggered(evt.title, evt.description);
        m_upgrade.addReputation(evt.impact * 0.1);
    };

    m_upgrade.onLevelUp = [this](const int level, const QString& name) {
        emit levelUp(level, name);
    };

    utils::Logger::log(1, "GameEngine initialized");
}

void GameEngine::loadConfigs(const QString& configDir) {
    QString cityPath = configDir + "/city_config.json";
    utils::Logger::log(0, "[GameEngine] Loading city config: " + cityPath.toStdString());
    auto cityJson = utils::loadJSON(cityPath.toStdString());
    utils::Logger::log(0, std::string("[GameEngine] City JSON keys: ") + std::to_string(cityJson.size()));
    m_grid.loadFromJSON(cityJson);

    QString companyPath = configDir + "/company_config.json";
    utils::Logger::log(0, "[GameEngine] Loading company config: " + companyPath.toStdString());
    auto companyJson = utils::loadJSON(companyPath.toStdString());
    utils::Logger::log(0, std::string("[GameEngine] Company JSON keys: ") + std::to_string(companyJson.size()));
    m_upgrade.loadFromJSON(companyJson);

    QString eventPath = configDir + "/event_config.json";
    utils::Logger::log(0, "[GameEngine] Loading event config: " + eventPath.toStdString());
    auto eventJson = utils::loadJSON(eventPath.toStdString());
    utils::Logger::log(0, std::string("[GameEngine] Event JSON keys: ") + std::to_string(eventJson.size()));
    m_eventSystem.loadFromJSON(eventJson);
}

void GameEngine::start() {
    m_lastFrameTime = utils::now();
    connect(&m_realTimer, &QTimer::timeout, this, &GameEngine::onTick);
    m_realTimer.start(100);  // 100ms = 10 FPS
    utils::Logger::log(1, "Game started");
}

void GameEngine::pause() {
    m_timer.pause();
}

void GameEngine::resume() {
    m_timer.resume();
    m_lastFrameTime = utils::now();
}

void GameEngine::onTick() {
    auto now = utils::now();
    double realDeltaMs = std::chrono::duration<double, std::milli>(now - m_lastFrameTime).count();
    m_lastFrameTime = now;

    double gameDeltaMs = m_timer.tick(realDeltaMs);
    double gameDeltaSec = gameDeltaMs / 1000.0;

    if (gameDeltaSec > 0) {
        m_gameTimeSeconds += static_cast<int>(gameDeltaSec);

        // 事件影响：负值减速，正值加速
        const int eventImpact = m_eventSystem.getTotalImpact();
        double effectiveDelta = gameDeltaSec * (1.0 + eventImpact / 100.0);
        if (effectiveDelta < 0.1) effectiveDelta = 0.1;  // 最低速度保底

        // 自动下单
        m_orderTimer += effectiveDelta;
        if (m_orderTimer >= m_orderInterval) {
            m_orderTimer = 0.0;
            generateRandomOrder();
        }

        m_dispatch.tick(effectiveDelta);
        m_eventSystem.tick(gameDeltaSec);

        emit tickUpdated();
    }
}

int GameEngine::createOrder(int merchantId, int customerId) {
    const auto* customer = m_grid.findCustomer(customerId);
    int vipLevel = customer ? customer->getVipLevel() : 0;

    int orderId = m_dispatch.createOrder(merchantId, customerId, vipLevel);
    if (orderId > 0) {
        emit orderCreated(orderId);
    }
    return orderId;
}

bool GameEngine::tryUpgrade() {
    bool ok = m_upgrade.tryUpgrade();
    if (ok) {
        emit moneyChanged(m_upgrade.getMoney());
    }
    return ok;
}

bool GameEngine::hireDeliver(const QString& name) {
    if (static_cast<int>(m_dispatch.getDelivers().size()) >= m_upgrade.getMaxDelivers()) {
        return false;
    }
    double cost = 300.0;  // 招募费用
    if (!m_upgrade.addMoney(-cost)) return false;

    models::Deliver d(static_cast<int>(m_dispatch.getDelivers().size()), name);
    // 放置在仓库位置（默认 10,10）
    d.setPosition(10, 10);
    m_dispatch.addDeliver(d);

    emit moneyChanged(m_upgrade.getMoney());
    return true;
}

void GameEngine::saveGame(const QString& path) {
    utils::json data;
    data["game_time"] = m_gameTimeSeconds;
    data["money"] = m_upgrade.getMoney();
    data["reputation"] = m_upgrade.getReputation();
    data["level"] = m_upgrade.getLevel();
    GameState::saveToFile(path, data);
}

void GameEngine::loadGame(const QString& path) {
    if (const auto data = GameState::loadFromFile(path); data.empty()) return;
    // TODO: 完整恢复状态
}

void GameEngine::generateRandomOrder() {
    const auto& merchants = m_grid.getMerchants();
    const auto& customers = m_grid.getCustomers();
    if (merchants.empty() || customers.empty()) return;

    const int mIdx = utils::randomInt(0, static_cast<int>(merchants.size()) - 1);
    const int cIdx = utils::randomInt(0, static_cast<int>(customers.size()) - 1);

    createOrder(merchants[mIdx].getId(), customers[cIdx].getId());
    utils::Logger::log(0, "[GameEngine] Auto order: merchant=" +
                       merchants[mIdx].getName().toStdString() +
                       " -> customer=" + customers[cIdx].getName().toStdString());
}

void GameEngine::printAlgorithmStats() const {
    utils::Logger::log(1, "========== 算法使用统计 ==========");
    utils::Logger::log(1, "  " + m_dispatch.pathFinder().algorithmName() +
                       ": 调用 " + std::to_string(m_dispatch.pathFinder().getCallCount()) + " 次");
    utils::Logger::log(1, "  " + m_dispatch.deliverSorter().algorithmName() +
                       ": 调用 " + std::to_string(m_dispatch.deliverSorter().getCallCount()) + " 次");
    utils::Logger::log(1, "  " + m_dispatch.orderMerger().algorithmName() +
                       ": 调用 " + std::to_string(m_dispatch.orderMerger().getCallCount()) + " 次");
    utils::Logger::log(1, "  " + m_dispatch.addressMatcher().algorithmName() +
                       ": 调用 " + std::to_string(m_dispatch.addressMatcher().getCallCount()) + " 次");
    utils::Logger::log(1, "  " + m_dispatch.compressor().algorithmName() +
                       ": 调用 " + std::to_string(m_dispatch.compressor().getCallCount()) + " 次");
    utils::Logger::log(1, "==================================");
}
