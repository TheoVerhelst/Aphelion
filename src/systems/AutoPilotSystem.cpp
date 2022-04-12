#include <SFML/System/Time.hpp>
#include <systems/AutoPilotSystem.hpp>
#include <components/components.hpp>
#include <components/Body.hpp>
#include <Scene.hpp>
#include <GameEvent.hpp>
#include <Input.hpp>

AutoPilotSystem::AutoPilotSystem(Scene& scene):
    _scene{scene} {
}

std::queue<std::pair<GameEvent, bool>> AutoPilotSystem::queueTriggerEvents() {
    std::queue<std::pair<GameEvent, bool>> queue;
    const EntityId playerId{_scene.findUnique<Player>()};
    ShipControl& shipControl{_scene.getComponent<ShipControl>(playerId)};
    // First, check if the player is using RCS by themselves. If so, we should
    // desactivate the actions that the user is not doing themselves
    if (shipControl.playerRcsClockwise or shipControl.playerRcsCounterClockwise) {
        if (shipControl.autoRcsClockwise and not shipControl.playerRcsClockwise) {
            // Stop RCS clockwise
            GameEvent event{GameEventType::RcsClockwise, playerId, 0};
            queue.emplace(event, false);
        }
        if (shipControl.autoRcsCounterClockwise and not shipControl.playerRcsCounterClockwise) {
            // Stop RCS counterclockwise
            GameEvent event{GameEventType::RcsCounterClockwise, playerId, 0};
            queue.emplace(event, false);
        }
        shipControl.autoRcsClockwise = false;
        shipControl.autoRcsCounterClockwise = false;
        return queue;
    }

    // Check if the RCS should start or stop
    const Body& playerBody{_scene.getComponent<Body>(playerId)};
    const float threshold{shipControl.angularVelocityThreshold};
    if (playerBody.angularVelocity > threshold and not shipControl.autoRcsCounterClockwise) {
        // Start RCS counterclockwise
        GameEvent event{GameEventType::RcsCounterClockwise, playerId, 0};
        queue.emplace(event, true);
        shipControl.autoRcsCounterClockwise = true;
    } else if (playerBody.angularVelocity < -threshold and not shipControl.autoRcsClockwise) {
        // Start RCS clockwise
        GameEvent event{GameEventType::RcsClockwise, playerId, 0};
        queue.emplace(event, true);
        shipControl.autoRcsClockwise = true;
    } else if (std::abs(playerBody.angularVelocity) < threshold) {
        if (shipControl.autoRcsClockwise) {
            // Stop RCS clockwise
            GameEvent event{GameEventType::RcsClockwise, playerId, 0};
            queue.emplace(event, false);
            shipControl.autoRcsClockwise = false;
        }
        if (shipControl.autoRcsCounterClockwise) {
            // Stop RCS counterclockwise
            GameEvent event{GameEventType::RcsCounterClockwise, playerId, 0};
            queue.emplace(event, false);
            shipControl.autoRcsCounterClockwise = false;
        }
    }
    return queue;
}
