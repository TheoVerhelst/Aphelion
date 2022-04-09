#include <systems/GameplaySystem.hpp>
#include <GameEvent.hpp>
#include <Scene.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

void GameplaySystem::handleGameEvent(const GameEvent& event, sf::Time dt) {
    if (event.status != EventStatus::Ongoing) {
        return;
    }
    
    Vector2f dv{0, 0};
    float dw{0};
    const float engineAccel{200};
    const float rcsLinearAccel{50};
    const float rcsCircularAccel{10};
    switch (event.type) {
    case GameEventType::Engine:
        dv += {0, -engineAccel};
        break;
    case GameEventType::RcsUp:
        dv += {0, -rcsLinearAccel};
        break;
    case GameEventType::RcsDown:
        dv += {0, rcsLinearAccel};
        break;
    case GameEventType::RcsLeft:
        dv += {-rcsLinearAccel, 0};
        break;
    case GameEventType::RcsRight:
        dv += {rcsLinearAccel, 0};
        break;
    case GameEventType::RcsClockwise:
        dw += rcsCircularAccel;
        break;
    case GameEventType::RcsCounterClockwise:
        dw -= rcsCircularAccel;
        break;
    default:
        break;
    }
    Body& body{_scene.getComponent<Body>(event.entity)};
    body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
    body.angularVelocity += dw * dt.asSeconds();
}
