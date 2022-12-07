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

enum class ControllerButton: unsigned int {
    A = 1,
    B,
    X,
    Y,
    L1,
    R1,
    Back,
    Start,
    L3,
    R3,
    L2,
    R2
};
NLOHMANN_JSON_SERIALIZE_ENUM(ControllerButton, {
	{ControllerButton::A, "A"},
	{ControllerButton::B, "B"},
	{ControllerButton::X, "X"},
	{ControllerButton::Y, "Y"},
	{ControllerButton::L1, "L1"},
	{ControllerButton::R1, "R1"},
	{ControllerButton::Back, "Back"},
	{ControllerButton::Start, "Start"},
	{ControllerButton::L3, "L3"},
	{ControllerButton::R3, "R3"},
	{ControllerButton::L2, "L2"},
	{ControllerButton::R2, "R2"}
})

#endif // INPUT_HPP
