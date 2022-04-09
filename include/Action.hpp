#ifndef ACTION_HPP
#define ACTION_HPP

#include <utility>
#include <json.hpp>

namespace sf {
    class Time;
}

enum class GameAction {
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

NLOHMANN_JSON_SERIALIZE_ENUM(GameAction, {
    {GameAction::Engine, "engine"},
    {GameAction::RcsUp, "rcsUp"},
    {GameAction::RcsDown, "rcsDown"},
    {GameAction::RcsLeft, "rcsLeft"},
    {GameAction::RcsRight, "rcsRight"},
    {GameAction::RcsClockwise, "rcsClockwise"},
    {GameAction::RcsCounterClockwise, "rcsCounterClockwise"},
    {GameAction::ZoomIn, "zoomIn"},
    {GameAction::ZoomOut, "zoomOut"},
    {GameAction::RotateView, "rotateView"},
    {GameAction::ToggleMap, "toggleMap"},
    {GameAction::Pause, "pause"}
})

enum class MapAction {
    ZoomIn,
    ZoomOut,
    Exit
};

#endif // ACTION_HPP
