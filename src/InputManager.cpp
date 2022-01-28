#include <SFML/Window/Event.hpp>
#include <InputManager.hpp>

std::vector<TriggerAction> InputManager::getTriggerActions(const sf::Event& event) {
    std::vector<TriggerAction> actions;
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _mapping.find(event.key.code);
        if (mappingIt != _mapping.end()) {
            actions.emplace_back(mappingIt->second, event.type == sf::Event::KeyPressed);
        }
    }
    return actions;
}

std::vector<Action> InputManager::getContinuousActions() {
    std::vector<Action> actions;
    for (auto& [key, action] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            actions.push_back(action);
        }
    }
    return actions;
}
