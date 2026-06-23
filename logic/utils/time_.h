#pragma once

#include <chrono>
#include <thread>

namespace delivery::utils {

    using GameClock = std::chrono::steady_clock;
    using GameTimePoint = GameClock::time_point;
    using GameDuration = GameClock::duration;

    inline GameTimePoint now() {
        return GameClock::now();
    }

    // 游戏内时间管理器（支持调速）
    class GameTimer {
    public:
        GameTimer() : m_speed(1.0), m_paused(false) {}

        void setSpeed(double speed) {
            m_speed = std::max(0.0, speed);
        }

        double speed() const { return m_speed; }

        void pause() { m_paused = true; }
        void resume() { m_paused = false; }
        bool isPaused() const { return m_paused; }

        // 每帧调用，返回经过的游戏时间（毫秒）
        double tick(double realDeltaMs) {
            if (m_paused || m_speed <= 0) {
                return 0.0;
            }
            return realDeltaMs * m_speed;
        }

    private:
        double m_speed = 1.0;
        bool m_paused = false;
    };

} // namespace delivery::utils