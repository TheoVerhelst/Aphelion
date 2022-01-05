#include <memory>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <RenderSystem.hpp>
#include <components.hpp>
#include <Animation.hpp>

RenderSystem::RenderSystem(Scene& scene):
    _scene{scene} {
}

void RenderSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (EntityId id : _scene.view<Trace>()) {
        target.draw(_scene.getComponent<Trace>(id).trace, states);
    }
    for (EntityId id : _scene.view<sf::Sprite>()) {
        target.draw(_scene.getComponent<sf::Sprite>(id), states);
    }
    for (EntityId id : _scene.view<sf::CircleShape>()) {
        target.draw(_scene.getComponent<sf::CircleShape>(id), states);
    }
    for (EntityId id : _scene.view<sf::ConvexShape>()) {
        target.draw(_scene.getComponent<sf::ConvexShape>(id), states);
    }
    for (EntityId id : _scene.view<AnimationComponent>()) {
        for (auto& pair : _scene.getComponent<AnimationComponent>(id)) {
            target.draw(pair.second, states);
        }
    }
}

void RenderSystem::update(const sf::Time& dt) {
    for (EntityId id : _scene.view<Body, sf::Sprite>()) {
        sf::Sprite& sprite{_scene.getComponent<sf::Sprite>(id)};
        Body& body{_scene.getComponent<Body>(id)};
        sprite.setPosition(static_cast<Vector2f>(body.position));
        sprite.setRotation(body.rotation * 180. / pi);
    }
    for (EntityId id : _scene.view<Body, sf::CircleShape>()) {
        sf::CircleShape& shape{_scene.getComponent<sf::CircleShape>(id)};
        Body& body{_scene.getComponent<Body>(id)};
        shape.setPosition(static_cast<Vector2f>(body.position));
        shape.setRotation(body.rotation * 180. / pi);
    }
    for (EntityId id : _scene.view<Body, sf::ConvexShape>()) {
        sf::ConvexShape& shape{_scene.getComponent<sf::ConvexShape>(id)};
        Body& body{_scene.getComponent<Body>(id)};
        shape.setPosition(static_cast<Vector2f>(body.position));
        shape.setRotation(body.rotation * 180. / pi);
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
    for (EntityId id : _scene.view<Body, AnimationComponent>()) {
        AnimationComponent& animations{_scene.getComponent<AnimationComponent>(id)};
        for (auto& [action, animation] : animations) {
            Body& body{_scene.getComponent<Body>(id)};
            animation.getSprite().setPosition(static_cast<Vector2f>(body.position));
            animation.getSprite().setRotation(body.rotation * 180. / pi);
            if (not animation.isStopped()) {
                animation.update(dt);
            }
        }
    }
}
