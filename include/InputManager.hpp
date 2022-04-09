#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>


template <typename Action>
class InputManager {
public:
    InputManager(const std::map<sf::Keyboard::Key, Action>& mapping);
    std::vector<std::pair<Action, bool>> getTriggerActions(const sf::Event& event);
    std::vector<Action> getContinuousActions();

private:
    const std::map<sf::Keyboard::Key, Action> _mapping;
};

template <typename Action>
InputManager<Action>::InputManager(const std::map<sf::Keyboard::Key, Action>& mapping):
    _mapping{mapping} {
}

template <typename Action>
std::vector<std::pair<Action, bool>> InputManager<Action>::getTriggerActions(const sf::Event& event) {
    std::vector<std::pair<Action, bool>> actions;
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _mapping.find(event.key.code);
        if (mappingIt != _mapping.end()) {
            actions.emplace_back(mappingIt->second, event.type == sf::Event::KeyPressed);
        }
    }
    return actions;
}

template <typename Action>
std::vector<Action> InputManager<Action>::getContinuousActions() {
    std::vector<Action> actions;
    for (auto& [key, action] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            actions.push_back(action);
        }
    }
    return actions;
}


#endif // INPUTMANAGER_HPP
