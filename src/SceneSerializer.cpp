#include <fstream>
#include <iomanip>
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
            const std::string className{components.at("class").get<std::string>()};
            _loadedClasses.emplace(id, className);
            json entityClass(_entityClasses.at(className));
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

void SceneSerializer::save(const std::string& filename) const {
    json j;


    std::ofstream(filename) << std::setw(4) << j << std::endl;
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
    // TODO define a macro to automatically serialize with default values
    Sprite& sprite{_scene.assignComponent<Sprite>(id)};
    value.get_to(sprite);
    sprite.sprite.setTexture(_textureManager.get(sprite.texture));
    if (sprite.rect != sf::IntRect(0, 0, 0, 0)) {
        sprite.sprite.setTextureRect(sprite.rect);
    }
    sprite.sprite.setOrigin(_scene.getComponent<Body>(id).centerOfMass - sprite.offset);
}

void SceneSerializer::loadAnimations(const json& value, EntityId id) {
    Animations& animations{_scene.assignComponent<Animations>(id)};
    value.get_to(animations);
    for (auto& [action, animationData] : animations) {
        // Construct the animation object
        animationData.animation = Animation(_textureManager.get(animationData.texture), animationData.frames,
                _soundBufferManager.get(animationData.soundBuffer),
                animationData.soundLoopStart, animationData.soundLoopEnd);
        // Set the sprite origin
        const Vector2f spriteOrigin{_scene.getComponent<Body>(id).centerOfMass - animationData.offset};
        animationData.animation.getSprite().setOrigin(spriteOrigin);
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
    value.get_to(mapElement);
    mapElement.icon = tgui::Picture::create(_tguiTextureManager.get(mapElement.tguiTexture));
    mapElement.icon->setOrigin(0.5f, 0.5f);
    if (mapElement.type == MapElementType::CelestialBody) {
        mapElement.icon->setScale(2.f);
    }
}
