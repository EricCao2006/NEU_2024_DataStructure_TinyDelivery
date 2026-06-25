#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace delivery::utils {

    enum class LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    inline std::string timestamp() {
        const auto now = std::chrono::system_clock::now();
        const auto time_t = std::chrono::system_clock::to_time_t(now);
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
           << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    inline void log(LogLevel level, const std::string& msg) {
        const char* levelStr[] = {"[DEBUG]", "[INFO] ", "[WARN] ", "[ERROR]"};
        std::cerr << timestamp() << " " << levelStr[static_cast<int>(level)] << " " << msg << std::endl;
    }

    //TODO：教师要求使用算法时输出对应的[INFO]
#define LOG_DEBUG(msg) delivery::utils::log(delivery::utils::LogLevel::DEBUG, msg)
#define LOG_INFO(msg)  delivery::utils::log(delivery::utils::LogLevel::INFO, msg)
#define LOG_WARN(msg)  delivery::utils::log(delivery::utils::LogLevel::WARN, msg)
#define LOG_ERROR(msg) delivery::utils::log(delivery::utils::LogLevel::ERROR, msg)

} // namespace delivery::utils