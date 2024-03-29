#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>
#include <vector>
#include <filesystem>

class Paths {
public:
    static std::filesystem::path getFontsDirectory();
    static std::filesystem::path savePathFromStem(const std::string& stem);
    static std::filesystem::path getNewGameSavePath();
    static std::filesystem::path getSettingsPath();
    // Precondition: getSavePaths().empty() must be false
    static std::filesystem::path getMostRecentSavePath();
    static std::string generateStem();
    static std::vector<std::filesystem::path> getSavePaths();
    static std::vector<std::filesystem::path> getEntityPaths();
    static std::vector<std::filesystem::path> getTguiTexturePaths();
    static std::vector<std::filesystem::path> getTexturePaths();
    static std::vector<std::filesystem::path> getMusicPaths();
    static std::vector<std::filesystem::path> getShaderPaths();
    static std::vector<std::filesystem::path> getSoundPaths();
    static std::filesystem::path getBlackBodyDataPath();

private:
    static const std::filesystem::path _saveDirectory;
    static const std::filesystem::path _resourceDirectory;

    static std::vector<std::filesystem::path> getPaths(const std::string& directory, const std::string& extension);
    static bool comparePath(const std::filesystem::path& p1, const std::filesystem::path& p2);
};

#endif // PATHS_HPP
