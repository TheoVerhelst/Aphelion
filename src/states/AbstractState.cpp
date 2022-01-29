#include <states/AbstractState.hpp>
#include <states/StateStack.hpp>

AbstractState::AbstractState(StateStack& stack):
    _stack{stack} {
}
