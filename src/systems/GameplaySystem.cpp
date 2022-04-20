#include <systems/GameplaySystem.hpp>
#include <Event.hpp>
#include <Scene.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

void GameplaySystem::update(sf::Time dt) {
    for (auto& [id, player, body] : _scene.view<Player, Body>()) {
        Vector2f dv{0, 0};
        float dw{0};
        const float engineAccel{200};
        const float rcsLinearAccel{50};
        const float rcsCircularAccel{10};
        if (player.playerControls.engine or player.autoControls.engine) {
            dv += {0, -engineAccel};
        }
        if (player.playerControls.rcsUp or player.autoControls.rcsUp) {
            dv += {0, -rcsLinearAccel};
        }
        if (player.playerControls.rcsDown or player.autoControls.rcsDown) {
            dv += {0, rcsLinearAccel};
        }
        if (player.playerControls.rcsLeft or player.autoControls.rcsLeft) {
            dv += {-rcsLinearAccel, 0};
        }
        if (player.playerControls.rcsRight or player.autoControls.rcsRight) {
            dv += {rcsLinearAccel, 0};
        }
        if (player.playerControls.rcsClockwise or player.autoControls.rcsClockwise) {
            dw += rcsCircularAccel;
        }
        if (player.playerControls.rcsCounterClockwise or player.autoControls.rcsCounterClockwise) {
            dw -= rcsCircularAccel;
        }
        body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
        body.angularVelocity += dw * dt.asSeconds();
    }
}
