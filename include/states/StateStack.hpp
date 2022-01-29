#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <TGUI/Widget.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <states/AbstractState.hpp>

// Forward declarations
namespace sf {
    class Time;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;


class StateStack {
public:
    template <typename... Args>
    using StateBuilder = std::function<AbstractState*(Args&...)>;

    StateStack(tgui::BackendGui& gui);

    template <typename T, typename... Args>
    void registerStateBuilder(const StateBuilder<Args...>& builder) {
        getStateBuilder<T, Args...>(builder);
    }

    template <typename T, typename... Args>
    void pushState(Args&... args) {
        AbstractState* state{getStateBuilder<T, Args...>()(args...)};
        tgui::Widget::Ptr widget{state->buildGui()};
        _gui.add(widget);
        // TODO maybe use brace-init for the state shared_ptr
        _stack.emplace_back(widget, std::shared_ptr<AbstractState>(state));
    }

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


    template <typename T, typename... Args>
    const StateBuilder<Args...>& getStateBuilder(const StateBuilder<Args...>& init = StateBuilder<Args...>()) {
        static StateBuilder<Args...> builder{init};
        return builder;
    }
};

#endif // STATESTACK_HPP
