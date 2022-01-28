#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <map>
#include <vector>
#include <SFML/Window/Keyboard.hpp>
#include <Action.hpp>

// Forward declarations
namespace sf {
    class Event;
}


class InputManager {
public:
    std::vector<TriggerAction> getTriggerActions(const sf::Event& event);
    std::vector<Action> getContinuousActions();

private:
    const std::map<sf::Keyboard::Key, Action> _mapping{
        {sf::Keyboard::Z, Action::RcsUp},
        {sf::Keyboard::Q, Action::RcsLeft},
        {sf::Keyboard::S, Action::RcsDown},
        {sf::Keyboard::D, Action::RcsRight},
        {sf::Keyboard::A, Action::RcsCounterClockwise},
        {sf::Keyboard::E, Action::RcsClockwise},
        {sf::Keyboard::Space, Action::Engine},
        {sf::Keyboard::LShift, Action::ZoomIn},
        {sf::Keyboard::RShift, Action::ZoomIn},
        {sf::Keyboard::LControl, Action::ZoomOut},
        {sf::Keyboard::RControl, Action::ZoomOut},
        {sf::Keyboard::M, Action::ToggleMap},
        {sf::Keyboard::LAlt, Action::RotateView}
    };
};

#endif // INPUTMANAGER_HPP
