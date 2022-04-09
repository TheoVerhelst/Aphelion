#ifndef ACTION_HPP
#define ACTION_HPP

#include <utility>
#include <json.hpp>

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
NLOHMANN_JSON_SERIALIZE_ENUM(Action, {
    {Action::Engine, "engine"},
    {Action::RcsUp, "rcsUp"},
    {Action::RcsDown, "rcsDown"},
    {Action::RcsLeft, "rcsLeft"},
    {Action::RcsRight, "rcsRight"},
    {Action::RcsClockwise, "rcsClockwise"},
    {Action::RcsCounterClockwise, "rcsCounterClockwise"},
    {Action::ZoomIn, "zoomIn"},
    {Action::ZoomOut, "zoomOut"},
    {Action::RotateView, "rotateView"},
    {Action::ToggleMap, "toggleMap"},
    {Action::Exit, "exit"}
})

typedef std::pair<Action, bool> TriggerAction;

#endif // ACTION_HPP
