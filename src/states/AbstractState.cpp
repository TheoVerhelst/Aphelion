#include <states/AbstractState.hpp>
#include <states/StateStack.hpp>

AbstractState::AbstractState(StateStack& stack):
    _stack{stack} {
}

tgui::Group::Ptr AbstractState::loadGuiFromFile(const std::string& filename) {
    tgui::Group::Ptr group{tgui::Group::create()};
    group->loadWidgetsFromFile(filename);
    return group;
}
