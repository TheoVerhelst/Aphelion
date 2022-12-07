#include <fstream>
#include <iomanip>
#include <Paths.hpp>
#include <Settings.hpp>

namespace fs = std::filesystem;

Settings Settings::loadSettings() {
    Settings settings;
    fs::path settingsPath{Paths::getSettingsPath()};
    if (fs::exists(settingsPath)) {
        nlohmann::json settingsJson(nlohmann::json::parse(std::ifstream(settingsPath)));
        settingsJson.get_to(settings);
    }
    return settings;
}

void Settings::saveSettings(const Settings& settings) {
    std::ofstream file{Paths::getSettingsPath()};
    file << std::setw(4) << nlohmann::json(settings) << std::endl;
}
