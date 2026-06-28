#include "GameState.h"

bool GameState::saveToFile(const QString& path, const utils::json& data) {
    return utils::saveJSON(path.toStdString(), data);
}

utils::json GameState::loadFromFile(const QString& path) {
    return utils::loadJSON(path.toStdString());
}
