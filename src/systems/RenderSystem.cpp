#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <systems/RenderSystem.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>
#include <components/Animations.hpp>
#include <vector.hpp>

RenderSystem::RenderSystem(Scene& scene):
    _scene{scene} {
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& [id, sprite] : _scene.view<Sprite>()) {
        target.draw(sprite.sprite, states);
    }
    for (auto& [id, animations] : _scene.view<Animations>()) {
        for (auto& [action, animationData] : animations) {
            if (not animationData.animation.isStopped()) {
                target.draw(animationData.animation.getSprite(), states);
            }
        }
    }
}

void RenderSystem::update() {
    for (auto& [id, body, sprite] : _scene.view<Body, Sprite>()) {
        sprite.sprite.setPosition(body.position);
        sprite.sprite.setRotation(radToDeg(body.rotation));
    }
    for (auto& [id, body, animations] : _scene.view<Body, Animations>()) {
        for (auto& [action, animationData] : animations) {
            animationData.animation.getSprite().setPosition(body.position);
            animationData.animation.getSprite().setRotation(radToDeg(body.rotation));
        }
    }
}
