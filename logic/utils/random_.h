#pragma once

#include <random>
#include <chrono>

namespace delivery::utils {

    inline std::mt19937& rng() {
        static std::mt19937 engine(
            static_cast<unsigned>(
                std::chrono::steady_clock::now().time_since_epoch().count()
            )
        );
        return engine;
    }

    // [min, max] 闭区间整数
    inline int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng());
    }

    // [min, max) 半开区间浮点数
    inline double randomDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(rng());
    }

    // 0~1 浮点数
    inline double random01() {
        return randomDouble(0.0, 1.0);
    }

    // 按概率 p 返回 true
    inline bool randomChance(double probability) {
        return random01() < probability;
    }

} // namespace delivery::utils