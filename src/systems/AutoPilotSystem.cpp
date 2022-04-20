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
    Player& player{_scene.getComponent<Player>(playerId)};
    // First, check if the player is using RCS by themselves. If so, we should
    // desactivate the actions that the user is not doing themselves
    if (player.playerControls.rcsClockwise or
        player.playerControls.rcsCounterClockwise) {
        if (player.autoControls.rcsClockwise and
            not player.playerControls.rcsClockwise) {
            // Stop RCS clockwise
            queue.emplace(playerId, false, Event::RcsEvent::Clockwise);
        }
        if (player.autoControls.rcsCounterClockwise and
            not player.playerControls.rcsCounterClockwise) {
            // Stop RCS counterclockwise
            queue.emplace(playerId, false, Event::RcsEvent::CounterClockwise);
        }
        player.autoControls.rcsClockwise = false;
        player.autoControls.rcsCounterClockwise = false;
        return queue;
    }

    // Check if the RCS should start or stop
    const Body& playerBody{_scene.getComponent<Body>(playerId)};
    const float threshold{player.angularVelocityThreshold};
    if (playerBody.angularVelocity > threshold and not player.autoControls.rcsCounterClockwise) {
        // Start RCS counterclockwise
        queue.emplace(playerId, true, Event::RcsEvent::CounterClockwise);
        player.autoControls.rcsCounterClockwise = true;
    } else if (playerBody.angularVelocity < -threshold and not player.autoControls.rcsClockwise) {
        // Start RCS clockwise
        queue.emplace(playerId, true, Event::RcsEvent::Clockwise);
        player.autoControls.rcsClockwise = true;
    } else if (std::abs(playerBody.angularVelocity) < threshold) {
        if (player.autoControls.rcsClockwise) {
            // Stop RCS clockwise
            queue.emplace(playerId, false, Event::RcsEvent::Clockwise);
            player.autoControls.rcsClockwise = false;
        }
        if (player.autoControls.rcsCounterClockwise) {
            // Stop RCS counterclockwise
            queue.emplace(playerId, false, Event::RcsEvent::CounterClockwise);
            player.autoControls.rcsCounterClockwise = false;
        }
    }
    return queue;
}
