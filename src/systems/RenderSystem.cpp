#include <memory>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <systems/RenderSystem.hpp>
#include <components.hpp>
#include <Animation.hpp>

RenderSystem::RenderSystem(Scene& scene, ResourceManager<sf::Shader>& shaderManager):
    _scene{scene},
    _shaderManager{shaderManager} {
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    drawComponent<Trace>(target, states);
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
    for (EntityId id : _scene.view<Body, Trace>()) {
        Body& body{_scene.getComponent<Body>(id)};
        Trace& trace{_scene.getComponent<Trace>(id)};
        std::size_t nextIndex{(trace.traceIndex + 1) % trace.traceLength};

        // The new trace line is from the previous one to the new position
        trace.trace[nextIndex * 2] = trace.trace[(trace.traceIndex * 2) + 1];
        trace.trace[nextIndex * 2 + 1] = sf::Vertex(
            static_cast<Vector2f>(body.position),
            sf::Color::White
        );
        // Adjust the trace transparency to make it fade away
        for (std::size_t j{0}; j < trace.traceLength; ++j) {
            float alpha{255.f * static_cast<float>(j) / static_cast<float>(trace.traceLength)};
            std::size_t index{((nextIndex + j) % trace.traceLength) * 2};
            trace.trace[index].color.a = static_cast<sf::Uint8>(alpha);
            trace.trace[index + 1].color.a = static_cast<sf::Uint8>(alpha);
        }
        trace.traceIndex = nextIndex;
    }
}

void RenderSystem::updateTransformable(sf::Transformable& transformable, EntityId id) const {
    Body& body{_scene.getComponent<Body>(id)};
    transformable.setPosition(static_cast<Vector2f>(body.position));
    transformable.setRotation(body.rotation * 180. / pi);
}
