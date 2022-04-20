#ifndef GAMEEVENT_HPP
#define GAMEEVENT_HPP

#include <cstdint>
#include <variant>

typedef std::uint32_t EntityId;

struct Event {
    struct EngineEvent {
    };

    enum class RcsEvent {
        Up, Down, Left, Right, Clockwise, CounterClockwise
    };

    struct CollisionEvent {
        float impactStrength;
        EntityId otherEntity;
    };

    EntityId entity;
    bool start;
    std::variant<EngineEvent, RcsEvent, CollisionEvent> data;
};

#endif // GAMEEVENT_HPP
