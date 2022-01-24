#ifndef SCENELOADER_HPP
#define SCENELOADER_HPP

#include <string>
#include <vector>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <json.hpp>
#include <vector.hpp>
#include <components.hpp>
#include <Scene.hpp>
#include <ResourceManager.hpp>

namespace tgui {
    class BackendGui;
    class Texture;
}

void loadScene(Scene& scene, const std::string& setupFile,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        tgui::BackendGui& gui);

void setupBody(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Font>& fontManager);
void setupCircleBody(Scene& scene, const nlohmann::json& value, EntityId id);
void setupConvexBody(Scene& scene, const nlohmann::json& value, EntityId id);
void setupSprite(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Texture>& textureManager);
void setupAnimations(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<sf::Texture>& textureManager);
void setupCircleShape(Scene& scene, const nlohmann::json& value, EntityId id);
void setupPlayer(Scene& scene, const nlohmann::json& value, EntityId id);
void setupLightSource(Scene& scene, const nlohmann::json& value, EntityId id);
void setupMapElement(Scene& scene, const nlohmann::json& value, EntityId id,
        const ResourceManager<tgui::Texture>& tguiTextureManager, tgui::BackendGui& gui);

#endif // SCENELOADER_HPP
