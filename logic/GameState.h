#pragma once

#include "utils/json_.h"

#include <QString>

/**
 * @brief 游戏存档
 */
class GameState {
public:
    GameState() = default;

    /// 保存到 JSON 文件
    static bool saveToFile(const QString& path, const utils::json& data);

    /// 从 JSON 文件加载
    static utils::json loadFromFile(const QString& path);
};
