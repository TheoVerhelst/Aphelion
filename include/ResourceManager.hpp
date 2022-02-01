#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <filesystem>
#include <unordered_map>
#include <TGUI/Texture.hpp>


template <typename T>
class ResourceManager {
public:
    template <typename... Args>
    void registerFromFile(const std::filesystem::path& path, const std::string& id, Args&&... args) {
        if (_resourceLoaders.contains(id)) {
            throw std::runtime_error("Resource " + id + " already registerd, not registering from " + path.string());
        }
        _resourceLoaders[id] = [this, id, path, &args...] {
            if(not _resources[id].loadFromFile(path.string(), std::forward<Args>(args)...)) {
                throw std::runtime_error("Error while loading resource " + id + " from " + path.string());
            }
        };
    }

    T& get(const std::string& id) {
        if (not _resources.contains(id)) {
            _resourceLoaders.at(id)();
            _resourceLoaders.erase(id);
        }
        return _resources.at(id);
    }

private:
    std::unordered_map<std::string, std::function<void()>> _resourceLoaders;
    std::unordered_map<std::string, T> _resources;
};


// tgui::Texture has a different interface to load from file
template <>
template <typename... Args>
void ResourceManager<tgui::Texture>::registerFromFile(const std::filesystem::path& path, const std::string& id, Args&&... args)  {
    if (_resourceLoaders.contains(id)) {
        throw std::runtime_error("Resource " + id + " already registerd, not registering from " + path.string());
    }
    _resourceLoaders[id] = [this, id, path, &args...] {
        _resources[id].load(path.string(), std::forward<Args>(args)...);
    };
}


#endif // RESOURCEMANAGER_HPP
