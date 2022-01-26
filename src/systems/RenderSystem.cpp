#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <systems/RenderSystem.hpp>
#include <ResourceManager.hpp>
#include <components.hpp>
#include <Animation.hpp>
#include <vector.hpp>

RenderSystem::RenderSystem(Scene& scene, ResourceManager<sf::Shader>& shaderManager):
    _scene{scene},
    _shaderManager{shaderManager} {
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    drawComponent<sf::Sprite>(target, states);
    drawComponent<AnimationComponent>(target, states);
}

void RenderSystem::update(const sf::Time& dt) {
    for (EntityId id : _scene.view<Body, sf::Sprite>()) {
        updateTransformable(_scene.getComponent<sf::Sprite>(id), id);
    }
    for (EntityId id : _scene.view<Body, AnimationComponent>()) {
        auto& animations{_scene.getComponent<AnimationComponent>(id).animations};
        for (auto& [action, animation] : animations) {
            updateTransformable(animation.getSprite(), id);
            if (not animation.isStopped()) {
                animation.update(dt);
            }
        }
    }
}

void RenderSystem::updateTransformable(sf::Transformable& transformable, EntityId id) const {
    const Body& body{_scene.getComponent<Body>(id)};
    transformable.setPosition(body.position);
    transformable.setRotation(radToDeg(body.rotation));
}
