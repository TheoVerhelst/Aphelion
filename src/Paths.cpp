#include <cassert>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <Paths.hpp>

namespace fs = std::filesystem;

const fs::path Paths::_saveDirectory{"saves"};
const fs::path Paths::_resourceDirectory{"resources"};


fs::path Paths::getFontsDirectory() {
    return _resourceDirectory/"fonts";
}

fs::path Paths::savePathFromStem(const std::string& stem) {
    return _saveDirectory / (stem + ".json");
}

fs::path Paths::getNewGameSavePath() {
    return _resourceDirectory/"newGame.json";
}

fs::path Paths::getMostRecentSavePath() {
    std::vector<fs::path> savePaths{getSavePaths()};
    assert(not savePaths.empty());
    return *std::max_element(savePaths.begin(), savePaths.end(), &comparePath);
}

std::string Paths::generateStem() {
    // Create a path based on the current date and time
    std::time_t now{std::time(nullptr)};
    std::tm nowCalendar{*std::localtime(&now)};
    std::stringstream formattedTime;
    formattedTime << std::put_time(&nowCalendar, "NewGame-%F-%H-%M-%S");
    return formattedTime.str();
}

std::vector<fs::path> Paths::getSavePaths() {
    if (not fs::exists(_saveDirectory)) {
        fs::create_directory(_saveDirectory);
    }
    return getPaths(_saveDirectory, ".json");
}

std::vector<fs::path> Paths::getEntityPaths() {
    return getPaths(_resourceDirectory/"entities", ".json");
}

std::vector<fs::path> Paths::getTguiTexturePaths() {
    return getPaths(_resourceDirectory/"gui", ".png");
}

std::vector<fs::path> Paths::getTexturePaths() {
    return getPaths(_resourceDirectory/"textures", ".png");
}

std::vector<fs::path> Paths::getMusicPaths() {
    return getPaths(_resourceDirectory/"musics", ".ogg");
}

std::vector<fs::path> Paths::getShaderPaths() {
    return getPaths(_resourceDirectory/"shaders", ".frag");
}

std::vector<fs::path> Paths::getSoundPaths() {
    return getPaths(_resourceDirectory/"sounds", ".wav");
}

std::vector<fs::path> Paths::getPaths(const std::string& directory, const std::string& extension) {
    std::vector<fs::path> res;
    for (auto const& entry : fs::directory_iterator{directory}) {
        if (entry.is_regular_file() and entry.path().extension() == extension) {
            res.push_back(entry);
        }
    }
    // Sort by recency
    std::sort(res.begin(), res.end(), &comparePath);
    return res;
}

bool Paths::comparePath(const fs::path& p1, const fs::path& p2) {
    return fs::last_write_time(p2) < fs::last_write_time(p1);
}
