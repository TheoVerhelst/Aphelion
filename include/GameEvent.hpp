#ifndef GAMEEVENT_HPP
#define GAMEEVENT_HPP

#include <cstdint>
#include <json.hpp>

typedef std::uint32_t EntityId;

enum class GameEventType {
    Engine,
    RcsUp,
    RcsDown,
    RcsLeft,
    RcsRight,
    RcsClockwise,
    RcsCounterClockwise,
    Collision
};

NLOHMANN_JSON_SERIALIZE_ENUM(GameEventType, {
    {GameEventType::Engine, "engine"},
    {GameEventType::RcsUp, "rcsUp"},
    {GameEventType::RcsDown, "rcsDown"},
    {GameEventType::RcsLeft, "rcsLeft"},
    {GameEventType::RcsRight, "rcsRight"},
    {GameEventType::RcsClockwise, "rcsClockwise"},
    {GameEventType::RcsCounterClockwise, "rcsCounterClockwise"},
    {GameEventType::Collision, "collision"}
})

struct GameEvent {
    GameEventType type;
    EntityId entity;
    EntityId otherEntity;
};

#endif // GAMEEVENT_HPP
