#include <states/StateStack.hpp>


StateStack::StateStack(tgui::BackendGui& gui) :
    _gui{gui} {
}

void StateStack::popStatesUpTo(const AbstractState& state) {
    _actionQueue.push([this, &state] {
        assert(not _stack.empty());
        while (not _stack.empty() and _stack.back().state.get() != &state) {
            _gui.remove(_stack.back().widget);
            _stack.pop_back();
        }
    });
}

void StateStack::popStatesUntil(const AbstractState& state) {
    _actionQueue.push([this, &state] {
        assert(not _stack.empty());
        bool foundState{false};
        while (not _stack.empty() and not foundState) {
            foundState = _stack.back().state.get() == &state;
            _gui.remove(_stack.back().widget);
            _stack.pop_back();
        }
    });
}

void StateStack::clearStates() {
    _actionQueue.push([this] {
        while (not _stack.empty()) {
            _gui.remove(_stack.back().widget);
            _stack.pop_back();
        }
    });
}

void StateStack::update(sf::Time dt) {
    while (not _actionQueue.empty()) {
        _actionQueue.front()();
        _actionQueue.pop();
    }
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        if (it->state->update(dt)) {
            break;
        }
    }
}

void StateStack::handleEvent(const sf::Event& event) {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        if (it->state->handleEvent(event)) {
            break;
        }
    }
}

void StateStack::handleContinuousInputs(sf::Time dt) {
    for (auto it = _stack.rbegin(); it != _stack.rend(); ++it) {
        if (it->state->handleContinuousInputs(dt)) {
            break;
        }
    }
}

bool StateStack::isEmpty() const {
    return _stack.empty();
}
