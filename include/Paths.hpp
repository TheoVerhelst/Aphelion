#ifndef PATHS_HPP
#define PATHS_HPP

#include <string>
#include <vector>
#include <filesystem>

class Paths {
public:
    static std::filesystem::path getFontsDirectory();
    static std::filesystem::path savePathFromStem(const std::string& stem);
    static std::vector<std::filesystem::path> getSavePaths();
    static std::vector<std::filesystem::path> getEntityPaths();
    static std::vector<std::filesystem::path> getTguiTexturePaths();
    static std::vector<std::filesystem::path> getTexturePaths();
    static std::vector<std::filesystem::path> getMusicPaths();
    static std::vector<std::filesystem::path> getShaderPaths();
    static std::vector<std::filesystem::path> getSoundsPaths();

private:
    static const std::filesystem::path _saveDirectory;
    static const std::filesystem::path _resourceDirectory;

    static std::vector<std::filesystem::path> getPaths(const std::string& directory, const std::string& extension);
};

#endif // PATHS_HPP
