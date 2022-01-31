#ifndef PATHS_HPP
#define PATHS_HPP

#include <vector>
#include <filesystem>

class Paths {
public:
    static std::vector<std::filesystem::path> getSavePaths();
    static std::filesystem::path savePathFromStem(const std::string& stem);

private:
    static const std::filesystem::path _saveDirectory;
};

#endif // PATHS_HPP
