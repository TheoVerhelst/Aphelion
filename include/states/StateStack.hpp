#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include <vector>
#include <queue>
#include <memory>
#include <utility>
#include <functional>
#include <TGUI/Widget.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <states/AbstractState.hpp>


class StateStack {
public:
    template <typename... Args>
    using StateBuilder = std::function<AbstractState*(Args...)>;

    StateStack(tgui::BackendGui& gui);

    template <typename T, typename... Args>
    void registerStateBuilder(const StateBuilder<Args...>& builder) {
        getStateBuilder<T, Args...>(builder);
    }

    template <typename T, typename... Args>
    void pushState(Args&&... args) {
        AbstractState* state{getStateBuilder<T, Args...>()(std::forward<Args>(args)...)};
        _actionQueue.push([this, state] {
            tgui::Widget::Ptr widget{state->buildGui()};
            _gui.add(widget);
            _stack.emplace_back(widget, std::shared_ptr<AbstractState>(state));
        });
    }

    void popState();
    void clear();
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
    std::queue<std::function<void()>> _actionQueue;

    template <typename T, typename... Args>
    const StateBuilder<Args...>& getStateBuilder(
            const StateBuilder<Args...>& init = [](Args...){assert(false);return nullptr;}) {
        static StateBuilder<Args...> builder{init};
        return builder;
    }
};

#endif // STATESTACK_HPP
