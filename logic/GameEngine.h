#pragma once
/**
 * @file GameEngine.h
 * @brief 游戏引擎 - 主循环与系统调度
 * @details 负责时间推进、订单生成、调度系统协调
 *          所有 subsystems 在此被调用
 */

#include "GameState.h"
#include "systems/DispatchSystem.h"
#include "systems/WarehouseSystem.h"
#include "systems/ContractSystem.h"
#include "systems/UpgradeSystem.h"
#include "systems/EventSystem.h"
#include "utils/time_.h"
#include <functional>
#include <memory>

namespace delivery {

/**
 * @brief 游戏引擎状态
 */
enum class EngineState : uint8_t {
    RUNNING,      ///< 运行中
    PAUSED,       ///< 暂停
    STOPPED       ///< 已停止
};

/**
 * @brief 游戏引擎
 * @note 单例模式，管理游戏主循环
 */
class GameEngine {
public:
    static GameEngine& instance();

    // ========== 禁止拷贝 ==========
    GameEngine(const GameEngine&) = delete;
    GameEngine& operator=(const GameEngine&) = delete;

    // ========== 生命周期 ==========
    /**
     * @brief 初始化引擎
     * @param loadSave 是否加载存档（true=加载，false=新建游戏）
     * @return 是否成功
     */
    bool initialize(bool loadSave = false);

    /**
     * @brief 启动游戏循环（阻塞）
     */
    void run();

    /**
     * @brief 单帧更新（非阻塞，用于与GUI集成）
     * @param deltaMs 真实时间增量（毫秒）
     */
    void update(double deltaMs);

    /**
     * @brief 停止引擎
     */
    void stop();

    // ========== 控制 ==========
    void pause();
    void resume();
    void togglePause();
    void setSpeed(double speed);

    // ========== 状态查询 ==========
    [[nodiscard]] EngineState state() const { return m_state; }
    [[nodiscard]] bool isPaused() const { return m_state == EngineState::PAUSED; }
    [[nodiscard]] bool isRunning() const { return m_state == EngineState::RUNNING; }
    [[nodiscard]] double speed() const { return m_gameTimer.speed(); }

    // ========== 回调 ==========
    /**
     * @brief 设置每帧回调（用于UI更新）
     * @param callback 回调函数
     */
    void setFrameCallback(std::function<void()> callback);

    /**
     * @brief 设置事件回调（用于UI弹窗）
     */
    void setEventCallback(std::function<void(const std::string& title, const std::string& msg)> callback);

private:
    GameEngine() = default;
    ~GameEngine() = default;

    /**
     * @brief 游戏主循环更新逻辑
     */
    void tick(double gameDeltaSeconds);

    /**
     * @brief 生成新订单
     */
    void generateOrders();

    /**
     * @brief 更新骑手位置
     */
    void updateDelivers(double deltaSeconds);

    /**
     * @brief 检查订单完成情况
     */
    void checkCompletedOrders();

    // ========== 成员 ==========
    EngineState m_state = EngineState::STOPPED;
    utils::GameTimer m_gameTimer;
    double m_accumulator = 0.0;                      ///< 累积时间（用于固定时间步长）

    // 子系统
    std::unique_ptr<DispatchSystem> m_dispatchSystem;
    std::unique_ptr<WarehouseSystem> m_warehouseSystem;
    std::unique_ptr<ContractSystem> m_contractSystem;
    std::unique_ptr<UpgradeSystem> m_upgradeSystem;
    std::unique_ptr<EventSystem> m_eventSystem;

    // 订单生成计数器
    double m_orderGenerationTimer = 0.0;

    // 回调
    std::function<void()> m_frameCallback;
    std::function<void(const std::string&, const std::string&)> m_eventCallback;

    // 常量
    static constexpr double FIXED_TIME_STEP = 0.1;   ///< 固定时间步长（秒）
    static constexpr double ORDER_GENERATE_INTERVAL = 5.0;  ///< 订单生成间隔（秒）
};

} // namespace delivery