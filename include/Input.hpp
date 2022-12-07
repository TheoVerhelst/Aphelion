#ifndef INPUT_HPP
#define INPUT_HPP

#include <json.hpp>

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
NLOHMANN_JSON_SERIALIZE_ENUM(GameInput, {
	{GameInput::Engine, "engine"},
	{GameInput::RcsUp, "rcsUp"},
	{GameInput::RcsDown, "rcsDown"},
	{GameInput::RcsLeft, "rcsLeft"},
	{GameInput::RcsRight, "rcsRight"},
	{GameInput::RcsClockwise, "rcsClockwise"},
	{GameInput::RcsCounterClockwise, "rcsCounterClockwise"},
	{GameInput::ZoomIn, "zoomIn"},
	{GameInput::ZoomOut, "zoomOut"},
	{GameInput::RotateView, "rotateView"},
	{GameInput::ToggleMap, "toggleMap"},
	{GameInput::Pause, "pause"}
})

enum class MapInput {
    ZoomIn,
    ZoomOut,
    Exit
};
NLOHMANN_JSON_SERIALIZE_ENUM(MapInput, {
	{MapInput::ZoomIn, "zoomIn"},
	{MapInput::ZoomOut, "zoomOut"},
	{MapInput::Exit, "exit"}
})

typedef unsigned int ControllerButton;

#endif // INPUT_HPP
