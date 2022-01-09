#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <unordered_map>

template <typename T>
class ResourceManager {
public:
    void loadFromFile(const std::string& filename, const std::string& id) {
        if (_resources.contains(id)) {
            throw std::runtime_error("Resource " + id + " already loaded, not loading from " + filename);
        }
        if(not _resources[id].loadFromFile(filename)) {
            throw std::runtime_error("Error while loading resource " + id + " from " + filename);
        }
    }

    T& get(const std::string& id) {
        return _resources.at(id);
    }

    const T& get(const std::string& id) const {
        return _resources.at(id);
    }

private:
    std::unordered_map<std::string, T> _resources;
};

#endif // RESOURCEMANAGER_HPP
