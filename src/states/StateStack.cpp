#include <algorithm>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <states/AbstractState.hpp>
#include <states/StateStack.hpp>


StateStack::StateStack(tgui::BackendGui& gui) :
    _gui{gui} {
}

void StateStack::pushState(AbstractState* state) {
    tgui::Widget::Ptr widget{state->buildGui()};
    _gui.add(widget);
    // TODO maybe use brace-init for the state shared_ptr
    _stack.emplace_back(widget, std::shared_ptr<AbstractState>(state));
}

void StateStack::popState() {
    _popQueue++;
}

void StateStack::update(sf::Time dt) {
    // Remove states that asked to be removed
    while (_popQueue > 0) {
        _gui.remove(_stack.back().widget);
        _stack.pop_back();
        _popQueue--;
    }
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        it->state->update(dt);
    }
}

void StateStack::handleTriggerAction(const TriggerAction& action) {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        if (it->state->handleTriggerAction(action)) {
            break;
        }
    }
}

void StateStack::handleContinuousAction(const Action& action, sf::Time dt) {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        if (it->state->handleContinuousAction(action, dt)) {
            break;
        }
    }
}
