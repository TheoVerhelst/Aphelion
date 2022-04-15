#include <SFML/System/Time.hpp>
#include <systems/AutoPilotSystem.hpp>
#include <components/components.hpp>
#include <components/Body.hpp>
#include <Scene.hpp>
#include <Event.hpp>
#include <Input.hpp>

AutoPilotSystem::AutoPilotSystem(Scene& scene):
    _scene{scene} {
}

std::queue<Event> AutoPilotSystem::queueEvents() {
    std::queue<Event> queue;
    const EntityId playerId{_scene.findUnique<Player>()};
    ShipControl& shipControl{_scene.getComponent<ShipControl>(playerId)};
    // First, check if the player is using RCS by themselves. If so, we should
    // desactivate the actions that the user is not doing themselves
    if (shipControl.playerControls.rcsClockwise or
        shipControl.playerControls.rcsCounterClockwise) {
        if (shipControl.autoControls.rcsClockwise and
            not shipControl.playerControls.rcsClockwise) {
            // Stop RCS clockwise
            queue.emplace(playerId, false, Event::RcsEvent::Clockwise);
        }
        if (shipControl.autoControls.rcsCounterClockwise and
            not shipControl.playerControls.rcsCounterClockwise) {
            // Stop RCS counterclockwise
            queue.emplace(playerId, false, Event::RcsEvent::CounterClockwise);
        }
        shipControl.autoControls.rcsClockwise = false;
        shipControl.autoControls.rcsCounterClockwise = false;
        return queue;
    }

    // Check if the RCS should start or stop
    const Body& playerBody{_scene.getComponent<Body>(playerId)};
    const float threshold{shipControl.angularVelocityThreshold};
    if (playerBody.angularVelocity > threshold and not shipControl.autoControls.rcsCounterClockwise) {
        // Start RCS counterclockwise
        queue.emplace(playerId, true, Event::RcsEvent::CounterClockwise);
        shipControl.autoControls.rcsCounterClockwise = true;
    } else if (playerBody.angularVelocity < -threshold and not shipControl.autoControls.rcsClockwise) {
        // Start RCS clockwise
        queue.emplace(playerId, true, Event::RcsEvent::Clockwise);
        shipControl.autoControls.rcsClockwise = true;
    } else if (std::abs(playerBody.angularVelocity) < threshold) {
        if (shipControl.autoControls.rcsClockwise) {
            // Stop RCS clockwise
            queue.emplace(playerId, false, Event::RcsEvent::Clockwise);
            shipControl.autoControls.rcsClockwise = false;
        }
        if (shipControl.autoControls.rcsCounterClockwise) {
            // Stop RCS counterclockwise
            queue.emplace(playerId, false, Event::RcsEvent::CounterClockwise);
            shipControl.autoControls.rcsCounterClockwise = false;
        }
    }
    return queue;
}
