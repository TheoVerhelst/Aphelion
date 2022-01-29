#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include <vector>
#include <memory>
#include <utility>
#include <TGUI/Widget.hpp>

// Forward declarations
namespace sf {
    class Time;
}
namespace tgui {
    class BackendGui;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;
class AbstractState;

class StateStack {
public:
    StateStack(tgui::BackendGui& gui);
    void pushState(AbstractState* state);
    void popState();
    void update(sf::Time dt);
    void handleTriggerAction(const TriggerAction& action);
    void handleContinuousAction(const Action& action, sf::Time dt);
    bool isEmpty() const;

private:
    struct StatePair {
        tgui::Widget::Ptr widget;
        std::shared_ptr<AbstractState> state;
    };
    std::vector<StatePair> _stack;
    tgui::BackendGui& _gui;
    // Number of states to pop at the next update. We delay the removal to avoid
    // running code of deleted states when they call StateStack::popState.
    int _popQueue{0};
};

#endif // STATESTACK_HPP
