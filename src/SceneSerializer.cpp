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

void SceneSerializer::load(const std::filesystem::path& path) {
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
    json j(json::parse(std::ifstream(path)));
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

void SceneSerializer::save(const std::filesystem::path& path) const {
    json j;
    for (EntityId id : _scene.allEntities()) {
        json entityValue;
        saveComponent<Body>(entityValue, id, "body");
        saveComponent<CircleBody>(entityValue, id, "circleBody");
        saveConvexBody(entityValue, id, "convexBody");
        saveComponent<Sprite>(entityValue, id, "sprite");
        saveComponent<Animations>(entityValue, id, "animations");
        saveComponent<Player>(entityValue, id, "player");
        saveComponent<LightSource>(entityValue, id, "lightSource");
        saveComponent<MapElement>(entityValue, id, "mapElement");

        if (_loadedClasses.contains(id)) {
            const std::string className{_loadedClasses.at(id)};
            entityValue = computeClassPatch(_entityClasses.at(className), entityValue);
            entityValue["class"] = className;
        }

        j.push_back(entityValue);
    }
    std::ofstream file{path};
    file << std::setw(4) << j << std::endl;
}

void SceneSerializer::saveConvexBody(json& value, EntityId id, const std::string& name) const {
    if (_scene.hasComponent<ConvexBody>(id)) {
        // Copy the component
        ConvexBody convex{_scene.getComponent<ConvexBody>(id)};
        const Body& body{_scene.getComponent<Body>(id)};
        for (auto& vertex : convex.vertices) {
            vertex += body.centerOfMass;
        }
        value[name] = convex;
    }
}

json SceneSerializer::computeClassPatch(const json& classValue, const json& entityValue) const {
    // Find what values differ between the class and the actual value
    const json flatClass(classValue.flatten());
    json flatEntity(entityValue.flatten());
    float eps{1e-5f};
    for (auto& [path, classProperty] : flatClass.items()) {
        json::json_pointer pointer{path};
        assert(entityValue.contains(pointer));
        const json& entityProperty(entityValue[pointer]);
        if ((classProperty.is_number_float()
                and std::abs(classProperty.get<float>() - entityProperty.get<float>()) < eps)
                or classProperty == entityProperty) {
            // We can't erase from the pointer, so we erase the flat entity
            // and then we unflatten it
            flatEntity.erase(path);
        }
    }
    return flatEntity.unflatten();
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

void SceneSerializer::loadPlayer(const json& value, EntityId id) {
    Player& player{_scene.assignComponent<Player>(id)};
    value.get_to(player);
}

void SceneSerializer::loadLightSource(const json& value, EntityId id) {
    LightSource& lightSource{_scene.assignComponent<LightSource>(id)};
    value.get_to(lightSource);
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
