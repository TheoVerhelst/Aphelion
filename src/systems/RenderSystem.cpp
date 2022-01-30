#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <systems/RenderSystem.hpp>
#include <components.hpp>
#include <vector.hpp>

RenderSystem::RenderSystem(Scene& scene):
    _scene{scene} {
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (EntityId id : _scene.view<Sprite>()) {
        target.draw(_scene.getComponent<Sprite>(id).sprite, states);
    }
    for (EntityId id : _scene.view<Animations>()) {
        Animations& animations{_scene.getComponent<Animations>(id)};
        for (auto& [action, animationData] : animations) {
            if (not animationData.animation.isStopped()) {
                target.draw(animationData.animation.getSprite(), states);
            }
        }
    }
}

void RenderSystem::update() {
    for (EntityId id : _scene.view<Body, Sprite>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        Sprite& sprite{_scene.getComponent<Sprite>(id)};
        sprite.sprite.setPosition(body.position);
        sprite.sprite.setRotation(radToDeg(body.rotation));
    }
    for (EntityId id : _scene.view<Body, Animations>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        Animations& animations{_scene.getComponent<Animations>(id)};
        for (auto& [action, animationData] : animations) {
            animationData.animation.getSprite().setPosition(body.position);
            animationData.animation.getSprite().setRotation(radToDeg(body.rotation));
        }
    }
}
