#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <InputManager.hpp>


void InputManager::update(const sf::Time& dt) {
    for (auto& [key, action] : _mapping) {
        if (sf::Keyboard::isKeyPressed(key)) {
            ContinuousAction actionPair{action, dt};
            EventSource<const ContinuousAction&>::notifyObservers(actionPair);
        }
    }
}

bool InputManager::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed or event.type == sf::Event::KeyReleased) {
        auto mappingIt = _mapping.find(event.key.code);
        if (mappingIt != _mapping.end()) {
            TriggerAction actionPair{mappingIt->second, event.type == sf::Event::KeyPressed};
            EventSource<const TriggerAction&>::notifyObservers(actionPair);
            return true;
        }
    }
    return false;
}
