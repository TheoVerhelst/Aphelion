#ifndef AUTOPILOTSYSTEM_HPP
#define AUTOPILOTSYSTEM_HPP

#include <queue>
#include <utility>

// Forward declarations
class Scene;
struct GameEvent;

class AutoPilotSystem {
public:
    AutoPilotSystem(Scene& scene);
    std::queue<std::pair<GameEvent, bool>> queueTriggerEvents();

private:
    Scene& _scene;
};

#endif // AUTOPILOTSYSTEM_HPP
