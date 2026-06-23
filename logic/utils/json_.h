#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <filesystem>

namespace delivery::utils {

    using json = nlohmann::json;

    inline json loadJSON(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return json::object();
        }
        json data;
        file >> data;
        return data;
    }

    inline bool saveJSON(const std::string& path, const json& data) {
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << data.dump(4);
        return true;
    }

    inline bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }

} // namespace delivery::utils