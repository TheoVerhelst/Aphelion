#ifndef ACTION_HPP
#define ACTION_HPP

#include <utility>

namespace sf {
    class Time;
}

enum class Action {
    Engine,
    RcsUp,
    RcsDown,
    RcsLeft,
    RcsRight,
    RcsClockwise,
    RcsCounterClockwise,
    ZoomIn,
    ZoomOut,
    RotateView,
    ToggleMap,
    Exit
};

typedef std::pair<Action, bool> TriggerAction;
typedef std::pair<Action, sf::Time> ContinuousAction;

#endif // ACTION_HPP
