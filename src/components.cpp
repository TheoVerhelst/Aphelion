#include <SFML/Graphics/RenderTarget.hpp>
#include <components.hpp>

Vector2d Body::localToWorld(const Vector2d& point) const {
    return rotate(point, rotation) + position;
}

Vector2d Body::worldToLocal(const Vector2d& point) const {
    return rotate(point - position, -rotation);
}

void Trace::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(trace, states);
}

void AnimationComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& [acion, animation] : animations) {
        if(not animation.isStopped()) {
            target.draw(animation, states);
        }
    }
}
