#include <fstream>
#include <algorithm>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/TGUI.hpp>
#include <ResourceManager.hpp>
#include <Scene.hpp>
#include <components.hpp>
#include <SceneLoader.hpp>
#include <SceneLoader.hpp>
#include <Animation.hpp>
#include <DebugInfo.hpp>

using nlohmann::json;
using namespace std::placeholders;

void loadScene(Scene& scene, const std::string& setupFile,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager,
        tgui::BackendGui& gui) {
    std::ifstream file{setupFile};
	json j;
	try	{
	    j = json::parse(file);
	} catch (json::parse_error& ex) {
	    std::cerr << "parse error in " << setupFile << " at byte " << ex.byte << std::endl;
	}

    std::vector<std::pair<std::string, std::function<void(Scene&, const json&,  EntityId)>>> setupFunctions{
        {"body", std::bind(setupBody, _1, _2, _3, std::cref(fontManager))},
        {"circleBody", setupCircleBody},
        {"convexBody", setupConvexBody},
        {"sprite", std::bind(setupSprite, _1, _2, _3, std::cref(textureManager))},
        {"animations", std::bind(setupAnimations, _1, _2, _3, std::cref(textureManager), std::cref(soundBufferManager))},
        {"circleShape", setupCircleShape},
        {"player", setupPlayer},
        {"lightSource", setupLightSource},
        {"mapElement", std::bind(setupMapElement, _1, _2, _3, std::cref(tguiTextureManager), std::ref(gui))}
    };

	for (json& components : j.at("entities")) {
        EntityId id{scene.createEntity()};
        for (auto& [componentName, setupFunction] : setupFunctions) {
            if (components.contains(componentName)) {
                setupFunction(scene, components.at(componentName), id);
            }
        }
    }
}

void setupBody(Scene& scene, const json& value, EntityId id, const ResourceManager<sf::Font>& fontManager) {
    Body& body{scene.assignComponent<Body>(id)};
    value.get_to(body);
    scene.assignComponent<DebugInfo>(id, fontManager.get("debugFont"));
}

void setupCircleBody(Scene& scene, const json& value, EntityId id) {
    CircleBody& circle{scene.assignComponent<CircleBody>(id)};
    value.get_to(circle);
    scene.assignComponent<Shadow>(id);

    // Physical constants
    Body& body{scene.getComponent<Body>(id)};
    body.centerOfMass = circle.computeCenterOfMass();
    body.momentOfInertia =  circle.computeMomentOfInertia(body.mass);
    body.type = BodyType::Circle;

}

void setupConvexBody(Scene& scene, const json& value, EntityId id) {
    ConvexBody& convex{scene.assignComponent<ConvexBody>(id)};
    value.get_to(convex);
    scene.assignComponent<Collider>(id);
    scene.assignComponent<Shadow>(id);

    // Physical constants
    Body& body{scene.getComponent<Body>(id)};
    body.centerOfMass = convex.computeCenterOfMass();
    // Shift the vertices so they represent correct local coordinates around the
    // center of mass
    for (auto& vertex : convex.vertices) {
        vertex -= body.centerOfMass;
    }
    body.momentOfInertia =  convex.computeMomentOfInertia(body.mass, {0., 0.});
    body.type = BodyType::Convex;

}

void setupSprite(Scene& scene, const json& value, EntityId id, const ResourceManager<sf::Texture>& textureManager) {
    sf::Sprite& sprite{scene.assignComponent<sf::Sprite>(id)};
    std::string textureName{value.at("texture").get<std::string>()};
    sprite.setTexture(textureManager.get(textureName));
    if (value.contains("rect")) {
        sprite.setTextureRect(value.at("rect").get<sf::IntRect>());
    }
    Vector2f offset{0, 0};
    if (value.contains("offset")) {
        value.at("offset").get_to(offset);
    }
    sprite.setOrigin(scene.getComponent<Body>(id).centerOfMass - offset);
}

void setupAnimations(Scene& scene, const json& value, EntityId id,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager) {
    AnimationComponent& animations{scene.assignComponent<AnimationComponent>(id)};
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
        Animation animation{textureManager.get(textureName), frames,
                soundBufferManager.get(soundBufferName), loopStart, loopEnd};
        // Set the sprite origin
        Vector2f spriteOrigin{0, 0};
        spriteOrigin += scene.getComponent<Body>(id).centerOfMass;
        if (animationValue.contains("offset")) {
            spriteOrigin -= animationValue.at("offset").get<Vector2f>();
        }
        animation.getSprite().setOrigin(spriteOrigin);
        animations.animations.emplace(nameToAction.at(name), animation);
    }
}

void setupCircleShape(Scene& scene, const json& value, EntityId id) {
    sf::CircleShape& shape{scene.assignComponent<sf::CircleShape>(id)};
    CircleBody& circle{scene.getComponent<CircleBody>(id)};
    shape.setRadius(circle.radius);
    shape.setFillColor(value.at("color").get<sf::Color>());
    shape.setOrigin(circle.radius, circle.radius);
    shape.setPointCount(static_cast<std::size_t>(circle.radius));
}

void setupPlayer(Scene& scene, const json&, EntityId id) {
    scene.assignComponent<Player>(id);
}

void setupLightSource(Scene& scene, const json& value, EntityId id) {
    LightSource& lightSource{scene.assignComponent<LightSource>(id)};
    value.at("brightness").get_to(lightSource.brightness);
}

void setupMapElement(Scene& scene, const json& value, EntityId id, const ResourceManager<tgui::Texture>& tguiTextureManager, tgui::BackendGui& gui) {
    MapElement& mapElement{scene.assignComponent<MapElement>(id)};
    value.at("type").get_to(mapElement.type);
    std::string textureName{value.at("texture").get<std::string>()};
    mapElement.icon = tgui::Picture::create(tguiTextureManager.get(textureName));
    gui.add(mapElement.icon);
    mapElement.icon->setVisible(false);
    mapElement.icon->setOrigin(0.5, 0.5);
    mapElement.icon->moveToFront();
    if (mapElement.type == MapElementType::CelestialBody) {
        mapElement.icon->setScale(2.f);
    }
}
