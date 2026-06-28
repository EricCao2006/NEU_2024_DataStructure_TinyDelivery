#pragma once

#include <iostream>
using namespace std;

// 日志等级, 0-debug, 1-info, 2-warn, 3-error
#define LOG_LEVEL 1

namespace utils {

    class Logger {
        // 日志头, 0-debug, 1-info, 2-warn, 3-error
        static constexpr string logHead[4] = { "\033[34m[DEBUG]", "\033[32m[INFO]", "\033[33m[WARN]", "\033[31m[ERROR]" };

    public:
        /**
         * @brief 输出日志，级别小于LOG_LEVEL的不输出
         * @param level 日志级别
         * @param content 日志内容
         */
        static void log(const int level, const string& content) {
            if (LOG_LEVEL > level ) {}
            else cout << logHead[level] << content << endl << "\033[0m";
        }

    };
}