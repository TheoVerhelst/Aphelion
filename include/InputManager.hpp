#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

template <typename Input>
class InputManager {
public:
    InputManager(const std::map<sf::Keyboard::Key, Input>& mapping);
    std::vector<std::pair<Input, bool>> getTriggerInputs(const sf::Event& event);
    std::vector<Input> getContinuousInputs();

private:
    const std::map<sf::Keyboard::Key, Input> _mapping;
};

template <typename Input>
InputManager<Input>::InputManager(const std::map<sf::Keyboard::Key, Input>& mapping):
    _mapping{mapping} {
}

template <typename Input>
std::vector<std::pair<Input, bool>> InputManager<Input>::getTriggerInputs(const sf::Event& event) {
    std::vector<std::pair<Input, bool>> inputs;
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _mapping.find(event.key.code);
        if (mappingIt != _mapping.end()) {
            inputs.emplace_back(mappingIt->second, event.type == sf::Event::KeyPressed);
        }
    }
    return inputs;
}

template <typename Input>
std::vector<Input> InputManager<Input>::getContinuousInputs() {
    std::vector<Input> inputs;
    for (auto& [key, input] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            inputs.push_back(input);
        }
    }
    return inputs;
}


#endif // INPUTMANAGER_HPP
