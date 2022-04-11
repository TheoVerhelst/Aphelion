#include <states/AbstractState.hpp>
#include <states/StateStack.hpp>

AbstractState::AbstractState(StateStack& stack):
    _stack{stack} {
}

bool AbstractState::update(sf::Time) {
    return false;
}

bool AbstractState::handleEvent(const sf::Event&) {
    return false;
}

bool AbstractState::handleContinuousInputs(sf::Time) {
    return false;
}
