#ifndef INPUT_HPP
#define INPUT_HPP

#include <utility>

namespace sf {
    class Time;
}

enum class GameInput {
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
    Pause
};

enum class MapInput {
    ZoomIn,
    ZoomOut,
    Exit
};

#endif // INPUT_HPP
