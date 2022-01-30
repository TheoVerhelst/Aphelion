#include <fstream>
#include <algorithm>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/TGUI.hpp>
#include <ResourceManager.hpp>
#include <Scene.hpp>
#include <components.hpp>
#include <SceneSerializer.hpp>
#include <Animation.hpp>

using nlohmann::json;
using namespace std::placeholders;

SceneSerializer::SceneSerializer(Scene& scene,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager):
    _scene{scene},
    _textureManager{textureManager},
    _tguiTextureManager{tguiTextureManager},
    _soundBufferManager{soundBufferManager},
    _entityClasses(json::parse(std::ifstream("resources/entities/entities.json"))) {
}

void SceneSerializer::load(const std::string& filename) {
    std::vector<std::pair<std::string, std::function<void(SceneSerializer*, const json&,  EntityId)>>> loadFunctions{
        {"body", &SceneSerializer::loadBody},
        {"circleBody", &SceneSerializer::loadCircleBody},
        {"convexBody", &SceneSerializer::loadConvexBody},
        {"sprite", &SceneSerializer::loadSprite},
        {"animations", &SceneSerializer::loadAnimations},
        {"player", &SceneSerializer::loadPlayer},
        {"lightSource", &SceneSerializer::loadLightSource},
        {"mapElement", &SceneSerializer::loadMapElement}
    };

    // We use parenthesis init rather than brace init, otherwise the json value
    // is wrapped in a JSON array
    json j(json::parse(std::ifstream(filename)));
	for (json& components : j) {
        EntityId id{_scene.createEntity()};
        if (components.contains("class")) {
            json entityClass(_entityClasses.at(components.at("class").get<std::string>()));
            // Recursively merge entityClass and components
            entityClass.update(components, true);
            components = entityClass;
        }
        for (auto& [componentName, loadFunction] : loadFunctions) {
            if (components.contains(componentName)) {
                loadFunction(this, components.at(componentName), id);
            }
        }
    }
}

void SceneSerializer::loadBody(const json& value, EntityId id) {
    Body& body{_scene.assignComponent<Body>(id)};
    value.get_to(body);
}

void SceneSerializer::loadCircleBody(const json& value, EntityId id) {
    CircleBody& circle{_scene.assignComponent<CircleBody>(id)};
    value.get_to(circle);
    _scene.assignComponent<Shadow>(id);

    // Physical constants
    Body& body{_scene.getComponent<Body>(id)};
    body.centerOfMass = circle.computeCenterOfMass();
    body.momentOfInertia =  circle.computeMomentOfInertia(body.mass);
    body.type = BodyType::Circle;

}

void SceneSerializer::loadConvexBody(const json& value, EntityId id) {
    ConvexBody& convex{_scene.assignComponent<ConvexBody>(id)};
    value.get_to(convex);
    _scene.assignComponent<Collider>(id);
    _scene.assignComponent<Shadow>(id);

    // Physical constants
    Body& body{_scene.getComponent<Body>(id)};
    body.centerOfMass = convex.computeCenterOfMass();
    // Shift the vertices so they represent correct local coordinates around the
    // center of mass
    for (auto& vertex : convex.vertices) {
        vertex -= body.centerOfMass;
    }
    body.momentOfInertia =  convex.computeMomentOfInertia(body.mass, {0., 0.});
    body.type = BodyType::Convex;

}

void SceneSerializer::loadSprite(const json& value, EntityId id) {
    sf::Sprite& sprite{_scene.assignComponent<sf::Sprite>(id)};
    std::string textureName{value.at("texture").get<std::string>()};
    sprite.setTexture(_textureManager.get(textureName));
    if (value.contains("rect")) {
        sprite.setTextureRect(value.at("rect").get<sf::IntRect>());
    }
    Vector2f offset{0, 0};
    if (value.contains("offset")) {
        value.at("offset").get_to(offset);
    }
    sprite.setOrigin(_scene.getComponent<Body>(id).centerOfMass - offset);
}

void SceneSerializer::loadAnimations(const json& value, EntityId id) {
    AnimationComponent& animations{_scene.assignComponent<AnimationComponent>(id)};
    std::map<std::string, Action> nameToAction{
        {"engine", Action::Engine},
        {"rcsUp", Action::RcsUp},
        {"rcsDown", Action::RcsDown},
        {"rcsLeft", Action::RcsLeft},
        {"rcsRight", Action::RcsRight},
        {"rcsClockwise", Action::RcsClockwise},
        {"rcsCounterClockwise", Action::RcsCounterClockwise}
    };
    for (auto& [name, animationValue] : value.items()) {
        // Extract animation data from JSON
        std::string textureName{animationValue.at("texture").get<std::string>()};
        std::vector<AnimationFrame> frames;
        animationValue.at("frames").get_to(frames);
        std::string soundBufferName{animationValue.at("soundBuffer").get<std::string>()};
        sf::Time loopStart{animationValue.at("soundLoopStart").get<sf::Time>()};
        sf::Time loopEnd{animationValue.at("soundLoopEnd").get<sf::Time>()};
        // Construct the animation object
        Animation animation{_textureManager.get(textureName), frames,
                _soundBufferManager.get(soundBufferName), loopStart, loopEnd};
        // Set the sprite origin
        Vector2f spriteOrigin{0, 0};
        spriteOrigin += _scene.getComponent<Body>(id).centerOfMass;
        if (animationValue.contains("offset")) {
            spriteOrigin -= animationValue.at("offset").get<Vector2f>();
        }
        animation.getSprite().setOrigin(spriteOrigin);
        animations.animations.emplace(nameToAction.at(name), animation);
    }
}

void SceneSerializer::loadPlayer(const json&, EntityId id) {
    _scene.assignComponent<Player>(id);
}

void SceneSerializer::loadLightSource(const json& value, EntityId id) {
    LightSource& lightSource{_scene.assignComponent<LightSource>(id)};
    value.at("brightness").get_to(lightSource.brightness);
}

void SceneSerializer::loadMapElement(const json& value, EntityId id) {
    MapElement& mapElement{_scene.assignComponent<MapElement>(id)};
    value.at("type").get_to(mapElement.type);
    std::string textureName{value.at("texture").get<std::string>()};
    mapElement.icon = tgui::Picture::create(_tguiTextureManager.get(textureName));
    mapElement.icon->setOrigin(0.5, 0.5);
    if (mapElement.type == MapElementType::CelestialBody) {
        mapElement.icon->setScale(2.f);
    }
}
