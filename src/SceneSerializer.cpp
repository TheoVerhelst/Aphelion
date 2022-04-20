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
#include <components/Animations.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>
#include <SceneSerializer.hpp>
#include <Animation.hpp>

using nlohmann::json;
using namespace std::placeholders;

SceneSerializer::SceneSerializer(Scene& scene,
        ResourceManager<sf::Texture>& textureManager,
        ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::SoundBuffer>& soundBufferManager):
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
        {"polygonBody", &SceneSerializer::loadPolygonBody},
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
        saveComponent<PolygonBody>(entityValue, id, "polygonBody");
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
    value.get_to(_scene.assignComponent<Body>(id));
}

void SceneSerializer::loadCircleBody(const json& value, EntityId id) {
    CircleBody& circle{_scene.assignComponent<CircleBody>(id)};
    value.get_to(circle);
    circle = CircleBody(_scene.getComponent<Body>(id), circle.radius);
}

void SceneSerializer::loadPolygonBody(const json& value, EntityId id) {
    PolygonBody& polygon{_scene.assignComponent<PolygonBody>(id)};
    value.get_to(polygon);
    polygon = PolygonBody(_scene.getComponent<Body>(id), polygon.vertices);
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
    for (auto& [eventType, animationData] : animations) {
        // Construct the animation object
        animationData.animation = Animation(
            _textureManager.get(animationData.texture),
            animationData.frames,
            _soundBufferManager.get(animationData.soundBuffer),
            animationData.soundLoopStart,
            animationData.soundLoopEnd
        );
        // Set the sprite origin
        const Vector2f spriteOrigin{_scene.getComponent<Body>(id).centerOfMass - animationData.offset};
        animationData.animation.getSprite().setOrigin(spriteOrigin);
    }
}

void SceneSerializer::loadPlayer(const json& value, EntityId id) {
    value.get_to(_scene.assignComponent<Player>(id));
}

void SceneSerializer::loadLightSource(const json& value, EntityId id) {
    value.get_to(_scene.assignComponent<LightSource>(id));
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
