#include <Paths.hpp>

const std::filesystem::path Paths::_saveDirectory{"saves"};

std::vector<std::filesystem::path> Paths::getSavePaths() {
    std::vector<std::filesystem::path> res;
    for (auto const& entry : std::filesystem::directory_iterator{_saveDirectory}) {
        if (entry.is_regular_file()) {
            res.push_back(entry);
        }
    }
    return res;
}

std::filesystem::path Paths::savePathFromStem(const std::string& stem) {
    return _saveDirectory / (stem + ".json");
}
