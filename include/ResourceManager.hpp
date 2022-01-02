#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <SFML/Graphics.hpp>

template <typename T>
class ResourceManager {
public:
    std::weak_ptr<T> loadFromFile(const std::string& filename, const std::string& id) {
        if (not _resources.contains(id)) {
            std::shared_ptr<T> resource{new T()};
            if(not resource->loadFromFile(filename)) {
                throw std::runtime_error("Error loading resource from " + filename);
            }
            _resources[id] = resource;
        }
        return _resources[id];
    }

    std::weak_ptr<T> get(const std::string& id) {
        return _resources.at(id);
    }

private:
    std::unordered_map<std::string, std::shared_ptr<T>> _resources;
};

#endif // RESOURCEMANAGER_HPP
