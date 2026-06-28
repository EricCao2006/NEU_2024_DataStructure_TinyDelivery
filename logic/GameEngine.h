#pragma once

#include "systems/DispatchSystem.h"
#include "systems/EventSystem.h"
#include "systems/UpgradeSystem.h"
#include "models/CityGrid.h"
#include "utils/time_.h"
#include "utils/json_.h"

#include <QTimer>
#include <QString>

/**
 * @brief 游戏引擎 —— 统筹所有子系统
 */
class GameEngine : public QObject {
    Q_OBJECT
public:
    explicit GameEngine(QObject* parent = nullptr);

    /// 初始化：加载配置、构建地图
    void init(const QString& configDir);

    /// 开始游戏主循环
    void start();

    /// 暂停/恢复
    void pause();
    void resume();
    bool isPaused() const { return m_timer.isPaused(); }

    /// 设置游戏速度倍率
    void setSpeed(double speed) { m_timer.setSpeed(speed); }
    double speed() const { return m_timer.speed(); }

    /// 手动创建一个订单
    int createOrder(int merchantId, int customerId);

    /// 尝试升级公司
    bool tryUpgrade();

    /// 招募新骑手
    bool hireDeliver(const QString& name);

    // ── 子系统访问 ──
    systems::DispatchSystem& dispatch() { return m_dispatch; }
    systems::EventSystem& events() { return m_eventSystem; }
    systems::UpgradeSystem& upgrade() { return m_upgrade; }
    const models::CityGrid& grid() const { return m_grid; }
    const utils::GameTimer& gameTimer() const { return m_timer; }

    /// 保存/加载存档
    void saveGame(const QString& path);
    void loadGame(const QString& path);

    /// 输出算法使用统计
    void printAlgorithmStats() const;

signals:
    void tickUpdated();
    void orderCreated(int orderId);
    void orderCompleted(int orderId);
    void eventTriggered(const QString& title, const QString& desc);
    void levelUp(int newLevel, const QString& newName);
    void moneyChanged(double newMoney);
    void reputationChanged(double newRep);

private slots:
    void onTick();

private:
    models::CityGrid m_grid;
    systems::DispatchSystem m_dispatch;
    systems::EventSystem m_eventSystem;
    systems::UpgradeSystem m_upgrade;
    utils::GameTimer m_timer;

    QTimer m_realTimer;       // 真实时间定时器（驱动游戏循环）
    utils::GameTimePoint m_lastFrameTime;
    int m_gameTimeSeconds = 0;

    // 自动下单计时器
    double m_orderTimer = 0.0;
    double m_orderInterval = 2.0;  // 每2游戏秒生成一个订单

    void loadConfigs(const QString& configDir);
    void generateRandomOrder();
};
