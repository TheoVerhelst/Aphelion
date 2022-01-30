#ifndef SCENELOADER_HPP
#define SCENELOADER_HPP

#include <string>
#include <cstddef>
#include <filesystem>
#include <json.hpp>
#include <Scene.hpp>

// Forward declarations
namespace sf {
    class Texture;
    class SoundBuffer;
}
namespace tgui {
    class Texture;
}
typedef std::uint32_t EntityId;
template <typename T>
class ResourceManager;

class SceneSerializer {
public:
    SceneSerializer(Scene& scene,
            const ResourceManager<sf::Texture>& textureManager,
            const ResourceManager<tgui::Texture>& tguiTextureManager,
            const ResourceManager<sf::SoundBuffer>& soundBufferManager);

    void load(const std::filesystem::path& path);
    void save(const std::filesystem::path& path) const;

private:
    Scene& _scene;
    const ResourceManager<sf::Texture>& _textureManager;
    const ResourceManager<tgui::Texture>& _tguiTextureManager;
    const ResourceManager<sf::SoundBuffer>& _soundBufferManager;
    const nlohmann::json _entityClasses;
    std::map<EntityId, std::string> _loadedClasses;

    void loadBody(const nlohmann::json& value, EntityId id);
    void loadCircleBody(const nlohmann::json& value, EntityId id);
    void loadConvexBody(const nlohmann::json& value, EntityId id);
    void loadSprite(const nlohmann::json& value, EntityId id);
    void loadAnimations(const nlohmann::json& value, EntityId id);
    void loadPlayer(const nlohmann::json& value, EntityId id);
    void loadLightSource(const nlohmann::json& value, EntityId id);
    void loadMapElement(const nlohmann::json& value, EntityId id);

    template <typename T>
    void saveComponent(nlohmann::json& value, EntityId id, const std::string name) const {
        if (_scene.hasComponent<T>(id)) {
            value[name] = _scene.getComponent<T>(id);
        }
    }
    void saveConvexBody(nlohmann::json& value, EntityId id, const std::string& name) const;
    nlohmann::json computeClassPatch(const nlohmann::json& classValue, const nlohmann::json& entityValue) const;

};

#endif // SCENELOADER_HPP