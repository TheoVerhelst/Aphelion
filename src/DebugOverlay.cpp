#include <DebugOverlay.hpp>

DebugOverlay::DebugOverlay(PhysicsSystem& physicsSystem, const SceneView<Body, DebugInfo>& scene):
    _physicsSystem{physicsSystem},
    _scene{scene} {
}

void DebugOverlay::update() {
    if (_debugView) {
        for (EntityId id : _scene) {
            Body& body{_scene.getComponent<Body>(id)};
            DebugInfo& debugInfo{_scene.getComponent<DebugInfo>(id)};
            debugInfo.update(body);
        }
    }
}

void DebugOverlay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (_debugView) {
        for (EntityId id : _scene) {
            target.draw(_scene.getComponent<DebugInfo>(id), states);
        }
    }
}

bool DebugOverlay::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::F3) {
        _debugView = not _debugView;
        return true;
    }
    return false;
}
