#include <systems/GameplaySystem.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <Animation.hpp>
#include <components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

void GameplaySystem::update(const TriggerAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, start] = actionPair;
    // Play the player animations
    auto& animations{_scene.getComponent<AnimationComponent>(playerId).animations};
    auto animationIt = animations.find(action);
    if (animationIt != animations.end()) {
        if (start) {
            animationIt->second.start();
        } else {
            animationIt->second.stop();
        }
    }
}

void GameplaySystem::update(const ContinuousAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, dt] = actionPair;
    Vector2f dv{0, 0};
    float dw{0};
    const float engineAccel{200};
    const float rcsLinearAccel{50};
    const float rcsCircularAccel{10};
    switch (action) {
        case Action::Engine:
            dv += {0, -engineAccel};
            break;
        case Action::RcsUp:
            dv += {0, -rcsLinearAccel};
            break;
        case Action::RcsDown:
            dv += {0, rcsLinearAccel};
            break;
        case Action::RcsLeft:
            dv += {-rcsLinearAccel, 0};
            break;
        case Action::RcsRight:
            dv += {rcsLinearAccel, 0};
            break;
        case Action::RcsClockwise:
            dw += rcsCircularAccel;
            break;
        case Action::RcsCounterClockwise:
            dw -= rcsCircularAccel;
            break;
        default:
            break;
    }
    Body& body{_scene.getComponent<Body>(playerId)};
    body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
    body.angularVelocity += dw * dt.asSeconds();
}
