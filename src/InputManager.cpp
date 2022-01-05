#include <InputManager.hpp>

std::set<Action> InputManager::getActiveActions() {
    std::set<Action> actions;
    for (auto& [key, action] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            actions.insert(action);
        }
    }
    return actions;
}
