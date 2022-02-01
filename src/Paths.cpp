#include <Paths.hpp>

const std::filesystem::path Paths::_saveDirectory{"saves"};
const std::filesystem::path Paths::_resourceDirectory{"resources"};


std::filesystem::path Paths::getFontsDirectory() {
    return _resourceDirectory/"fonts";
}

std::filesystem::path Paths::savePathFromStem(const std::string& stem) {
    return _saveDirectory / (stem + ".json");
}

std::vector<std::filesystem::path> Paths::getSavePaths() {
    return getPaths(_saveDirectory, ".json");
}

std::vector<std::filesystem::path> Paths::getEntityPaths() {
    return getPaths(_resourceDirectory/"entities", ".json");
}

std::vector<std::filesystem::path> Paths::getTguiTexturePaths() {
    return getPaths(_resourceDirectory/"gui", ".png");
}

std::vector<std::filesystem::path> Paths::getTexturePaths() {
    return getPaths(_resourceDirectory/"textures", ".png");
}

std::vector<std::filesystem::path> Paths::getMusicPaths() {
    return getPaths(_resourceDirectory/"musics", ".ogg");
}

std::vector<std::filesystem::path> Paths::getShaderPaths() {
    return getPaths(_resourceDirectory/"shaders", ".frag");
}

std::vector<std::filesystem::path> Paths::getSoundsPaths() {
    return getPaths(_resourceDirectory/"sounds", ".wav");
}

std::vector<std::filesystem::path> Paths::getPaths(const std::string& directory, const std::string& extension) {
    std::vector<std::filesystem::path> res;
    for (auto const& entry : std::filesystem::directory_iterator{directory}) {
        if (entry.is_regular_file() and entry.path().extension() == extension) {
            res.push_back(entry);
        }
    }
    return res;
}
