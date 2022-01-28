#ifndef SCENELOADER_HPP
#define SCENELOADER_HPP

#include <string>
#include <vector>
#include <cstddef>
#include <json.hpp>

// Forward declarations
namespace sf {
    class Texture;
    class Font;
    class SoundBuffer;
}
namespace tgui {
    class Texture;
}
class Scene;
typedef std::uint32_t EntityId;
template <typename T>
class ResourceManager;


void loadScene(Scene& scene, const std::string& saveFile,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager);

void setupBody(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Font>& fontManager);
void setupCircleBody(Scene& scene, const nlohmann::json& value, EntityId id);
void setupConvexBody(Scene& scene, const nlohmann::json& value, EntityId id);
void setupSprite(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Texture>& textureManager);
void setupAnimations(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager);
void setupCircleShape(Scene& scene, const nlohmann::json& value, EntityId id);
void setupPlayer(Scene& scene, const nlohmann::json& value, EntityId id);
void setupLightSource(Scene& scene, const nlohmann::json& value, EntityId id);
void setupMapElement(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<tgui::Texture>& tguiTextureManager);

#endif // SCENELOADER_HPP
