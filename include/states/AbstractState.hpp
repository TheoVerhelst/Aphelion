#ifndef ABSTRACTSTATE_HPP
#define ABSTRACTSTATE_HPP

#include <utility>
#include <TGUI/Widget.hpp>

// Forward declarations
namespace sf {
    class Time;
    class Event;
}
class StateStack;

class AbstractState {
public:
    AbstractState(StateStack& stack);
    virtual ~AbstractState() = default;
    virtual tgui::Widget::Ptr buildGui() = 0;
    virtual bool update(sf::Time dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;

protected:
    StateStack& _stack;
};

#endif // ABSTRACTSTATE_HPP
