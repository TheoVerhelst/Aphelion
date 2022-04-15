#include <systems/GameplaySystem.hpp>
#include <Event.hpp>
#include <Scene.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

void GameplaySystem::update(sf::Time dt) {
    for (auto& [id, control, body] : _scene.view<ShipControl, Body>()) {
        Vector2f dv{0, 0};
        float dw{0};
        const float engineAccel{200};
        const float rcsLinearAccel{50};
        const float rcsCircularAccel{10};
        if (control.playerControls.engine or control.autoControls.engine) {
            dv += {0, -engineAccel};
        }
        if (control.playerControls.rcsUp or control.autoControls.rcsUp) {
            dv += {0, -rcsLinearAccel};
        }
        if (control.playerControls.rcsDown or control.autoControls.rcsDown) {
            dv += {0, rcsLinearAccel};
        }
        if (control.playerControls.rcsLeft or control.autoControls.rcsLeft) {
            dv += {-rcsLinearAccel, 0};
        }
        if (control.playerControls.rcsRight or control.autoControls.rcsRight) {
            dv += {rcsLinearAccel, 0};
        }
        if (control.playerControls.rcsClockwise or control.autoControls.rcsClockwise) {
            dw += rcsCircularAccel;
        }
        if (control.playerControls.rcsCounterClockwise or control.autoControls.rcsCounterClockwise) {
            dw -= rcsCircularAccel;
        }
        body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
        body.angularVelocity += dw * dt.asSeconds();
    }
}
