#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <set>
#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>
#include <Action.hpp>

class InputManager {
public:
    std::set<Action> getContinuousActions();
    std::map<Action, bool> getTriggerActions();
    bool handleEvent(const sf::Event& event);

private:
    std::map<sf::Keyboard::Key, Action> _mapping{
        {sf::Keyboard::Z, Action::RcsUp},
        {sf::Keyboard::Q, Action::RcsLeft},
        {sf::Keyboard::S, Action::RcsDown},
        {sf::Keyboard::D, Action::RcsRight},
        {sf::Keyboard::A, Action::RcsCounterClockwise},
        {sf::Keyboard::E, Action::RcsClockwise},
        {sf::Keyboard::Space, Action::Engine},
        {sf::Keyboard::LShift, Action::ZoomIn},
        {sf::Keyboard::LControl, Action::ZoomOut},
        {sf::Keyboard::M, Action::ToggleMap},
        {sf::Keyboard::LAlt, Action::RotateView}
    };
    std::map<Action, bool> _triggerActions;
};

#endif // INPUTMANAGER_HPP
