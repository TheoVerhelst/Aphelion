#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <Input.hpp>

template <typename Input>
class InputManager {
public:
    InputManager(
        const std::map<sf::Keyboard::Key, Input>& keyboardMapping,
        const std::map<ControllerButton, Input>& controllerMapping);
    std::vector<std::pair<Input, bool>> getInputEvents(const sf::Event& event);
    bool isActivated(Input input);

private:
    const std::map<sf::Keyboard::Key, Input>& _keyboardMapping;
    const std::map<ControllerButton, Input>& _controllerMapping;
};

template <typename Input>
InputManager<Input>::InputManager(
        const std::map<sf::Keyboard::Key, Input>& keyboardMapping,
        const std::map<ControllerButton, Input>& controllerMapping):
    _keyboardMapping{keyboardMapping},
    _controllerMapping{controllerMapping} {
}

template <typename Input>
std::vector<std::pair<Input, bool>> InputManager<Input>::getInputEvents(const sf::Event& event) {
    std::vector<std::pair<Input, bool>> inputs;
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _keyboardMapping.find(event.key.code);
        if (mappingIt != _keyboardMapping.end()) {
            inputs.emplace_back(mappingIt->second, event.type == sf::Event::KeyPressed);
        }
    }
    return inputs;
}

template <typename Input>
bool InputManager<Input>::isActivated(Input input) {
    for (auto& [key, otherInput] : _keyboardMapping) {
        if (input == otherInput) {
            return sf::Keyboard::isKeyPressed(key);
        }
    }
    return false;
}


#endif // INPUTMANAGER_HPP
