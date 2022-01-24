#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <unordered_map>
#include <TGUI/Texture.hpp>


template <typename T>
class ResourceManager {
public:
    template <typename... Args>
    void loadFromFile(const std::string& filename, const std::string& id, const Args&... args) {
        if (_resources.contains(id)) {
            throw std::runtime_error("Resource " + id + " already loaded, not loading from " + filename);
        }
        if(not _resources[id].loadFromFile(filename, args...)) {
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


// tgui::Texture has a different interface to load from file
template <>
template <typename... Args>
void ResourceManager<tgui::Texture>::loadFromFile(const std::string& filename, const std::string& id, const Args&... args)  {
    if (_resources.contains(id)) {
        throw std::runtime_error("Resource " + id + " already loaded, not loading from " + filename);
    }
    _resources[id].load(filename, args...);
}


#endif // RESOURCEMANAGER_HPP
