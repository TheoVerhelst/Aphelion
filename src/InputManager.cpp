#include <InputManager.hpp>

std::set<Action> InputManager::getContinuousActions() {
    std::set<Action> actions;
    for (auto& [key, action] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            actions.insert(action);
        }
    }
    return actions;
}

std::map<Action, bool> InputManager::getTriggerActions() {
    std::map<Action, bool> copy(_triggerActions);
    _triggerActions.clear();
    return copy;
}

bool InputManager::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _mapping.find(event.key.code);
        if (mappingIt != _mapping.end()) {
            const Action action{mappingIt->second};
            const bool start{event.type == sf::Event::KeyPressed};
            _triggerActions[action] = start;
            return true;
        }
    }
    return false;
}
