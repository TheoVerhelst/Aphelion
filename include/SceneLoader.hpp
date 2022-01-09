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

void loadScene(Scene& scene, const std::string& setupFile,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager);

void setupCircle(Scene& scene, EntityId id);

void setupConvex(Scene& scene, EntityId id);

void setupAnimations(Scene& scene, EntityId id, nlohmann::json animationJson, const ResourceManager<sf::Texture>& textureManager, bool hasBody);

Vector2d convexSupportFunction(const Body& body, const ConvexBody& convex, const Vector2d& direction);

Vector2d localToWorld(const Body& body, const Vector2d& point);

Vector2d computeCenterOfMass(const std::vector<Vector2d>& vertices);

double computeMomentOfInertia(double mass, const Vector2d& centerOfMass, const std::vector<Vector2d>& vertices);


#endif // SCENELOADER_HPP
