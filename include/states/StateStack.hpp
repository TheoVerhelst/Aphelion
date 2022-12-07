#ifndef STATESTACK_HPP
#define STATESTACK_HPP

#include <vector>
#include <queue>
#include <memory>
#include <any>
#include <typeinfo>
#include <typeindex>
#include <utility>
#include <functional>
#include <TGUI/Widget.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <states/AbstractState.hpp>


class StateStack {
public:
    StateStack(tgui::BackendGui& gui);

    template <typename T, typename... Remaining, typename... InitArgs>
    void registerStateBuilder(InitArgs&&... initArgs) {
        std::function<T*(Remaining...)> stateBuilder{[&initArgs...] (Remaining&&... remaining) {
            return new T(std::forward<InitArgs>(initArgs)..., std::forward<Remaining>(remaining)...);
        }};
        _stateBuilders[std::type_index(typeid(T))] = std::any(stateBuilder);
    }

    template <typename T, typename... Args>
    void pushState(Args&&... args) {
        const auto& stateBuilder = std::any_cast<std::function<T*(Args...)>>(
            _stateBuilders.at(std::type_index(typeid(T)))
        );
        AbstractState* state{stateBuilder(std::forward<Args>(args)...)};
        _actionQueue.push([this, state] {
            tgui::Widget::Ptr widget{state->buildGui()};
            _gui.add(widget);
            _stack.emplace_back(widget, std::shared_ptr<AbstractState>(state));
        });
    }

    // Removes states from the stack up to but not including the given
    // state. The stack cannot be empty.
    void popStatesUpTo(const AbstractState& state);
    // Removes states from the stack up to and including the given
    // state. The stack cannot be empty.
    void popStatesUntil(const AbstractState& state);
    // Remove all states from the stack, if any.
    void clearStates();
    void update(sf::Time dt);
    void handleEvent(const sf::Event& event);
    void handleContinuousInputs(sf::Time dt);
    bool isEmpty() const;

private:
    struct StatePair {
        tgui::Widget::Ptr widget;
        std::shared_ptr<AbstractState> state;
    };

    std::vector<StatePair> _stack;
    tgui::BackendGui& _gui;
    std::queue<std::function<void()>> _actionQueue;
    std::map<std::type_index, std::any> _stateBuilders;
};

#endif // STATESTACK_HPP
