#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <Action.hpp>
#include <Observer.hpp>

// Forward declarations
namespace sf {
    class Time;
    class Event;
}
class Scene;


class InputManager : public EventSource<const ContinuousAction&>, public EventSource<const TriggerAction&>, public Observer<const sf::Time&>{
public:
    bool handleEvent(const sf::Event& event);
    virtual void update(const sf::Time& dt) override;

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
        {sf::Keyboard::LControl, Action::ZoomOut},
        {sf::Keyboard::M, Action::ToggleMap},
        {sf::Keyboard::LAlt, Action::RotateView}
    };
};

#endif // INPUTMANAGER_HPP
