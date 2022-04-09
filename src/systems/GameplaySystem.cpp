#include <systems/GameplaySystem.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

bool GameplaySystem::handleContinuousAction(const GameAction& action, sf::Time dt) {
    const EntityId playerId{_scene.findUnique<Player>()};
    Vector2f dv{0, 0};
    float dw{0};
    const float engineAccel{200};
    const float rcsLinearAccel{50};
    const float rcsCircularAccel{10};
    bool consumedAction{true};
    switch (action) {
        case GameAction::Engine:
            dv += {0, -engineAccel};
            break;
        case GameAction::RcsUp:
            dv += {0, -rcsLinearAccel};
            break;
        case GameAction::RcsDown:
            dv += {0, rcsLinearAccel};
            break;
        case GameAction::RcsLeft:
            dv += {-rcsLinearAccel, 0};
            break;
        case GameAction::RcsRight:
            dv += {rcsLinearAccel, 0};
            break;
        case GameAction::RcsClockwise:
            dw += rcsCircularAccel;
            break;
        case GameAction::RcsCounterClockwise:
            dw -= rcsCircularAccel;
            break;
        default:
            consumedAction = false;
            break;
    }
    Body& body{_scene.getComponent<Body>(playerId)};
    body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
    body.angularVelocity += dw * dt.asSeconds();
    return consumedAction;
}
