#include <memory>
#include <cmath>
#include <cassert>
#include <SFML/Graphics.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <systems/RenderSystem.hpp>
#include <components.hpp>
#include <Animation.hpp>

RenderSystem::RenderSystem(Scene& scene, ResourceManager<sf::Shader>& shaderManager):
    _scene{scene},
    _shaderManager{shaderManager} {
}

void RenderSystem::setRenderTarget(const sf::RenderTarget& renderTarget) {
    _renderTarget = &renderTarget;
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    drawComponent<sf::CircleShape>(target, states);
    drawComponent<sf::ConvexShape>(target, states);
    drawComponent<sf::Sprite>(target, states);
    drawComponent<AnimationComponent>(target, states);
}

void RenderSystem::update(const sf::Time& dt) {
    for (EntityId id : _scene.view<Body, sf::Sprite>()) {
        updateTransformable(_scene.getComponent<sf::Sprite>(id), id);
    }
    for (EntityId id : _scene.view<Body, sf::CircleShape>()) {
        updateTransformable(_scene.getComponent<sf::CircleShape>(id), id);
    }
    for (EntityId id : _scene.view<Body, sf::ConvexShape>()) {
        updateTransformable(_scene.getComponent<sf::ConvexShape>(id), id);
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
    updateMapElements();
}

void RenderSystem::updateTransformable(sf::Transformable& transformable, EntityId id) const {
    const Body& body{_scene.getComponent<Body>(id)};
    transformable.setPosition(static_cast<Vector2f>(body.position));
    transformable.setRotation(static_cast<float>(body.rotation * 180. / pi));
}

void RenderSystem::updateMapElements() {
    for (EntityId id : _scene.view<Body, MapElement>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        MapElement& mapElement{_scene.getComponent<MapElement>(id)};

        // Compute the position of the map element on the screen. Note that we
        // don't rotate the map icon.
        assert(_renderTarget != nullptr);
        Vector2i screenPosition{_renderTarget->mapCoordsToPixel(static_cast<Vector2f>(body.position))};
        mapElement.icon->setPosition(tgui::Vector2f(static_cast<Vector2f>(screenPosition)));
        // Except for ship icons
        if (mapElement.type == MapElementType::Ship) {
            float rotation{static_cast<float>(body.rotation * 180. / pi) - _renderTarget->getView().getRotation()};
            mapElement.icon->setRotation(rotation);
        }
    }
}
