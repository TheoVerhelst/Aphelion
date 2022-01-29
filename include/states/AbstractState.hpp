#ifndef ABSTRACTSTATE_HPP
#define ABSTRACTSTATE_HPP

#include <utility>
#include <TGUI/Widget.hpp>

// Forward declarations
namespace sf {
    class Time;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;
class StateStack;

class AbstractState {
public:
    AbstractState(StateStack& stack);
    virtual ~AbstractState() = default;
    virtual tgui::Widget::Ptr buildGui() = 0;
    virtual bool update(sf::Time dt) = 0;
    virtual bool handleTriggerAction(const TriggerAction& action) = 0;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) = 0;

protected:
    StateStack& _stack;
};

#endif // ABSTRACTSTATE_HPP
