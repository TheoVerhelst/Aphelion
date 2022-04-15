#ifndef AUTOPILOTSYSTEM_HPP
#define AUTOPILOTSYSTEM_HPP

#include <queue>
#include <utility>

// Forward declarations
class Scene;
struct Event;

class AutoPilotSystem {
public:
    AutoPilotSystem(Scene& scene);
    std::queue<Event> queueEvents();

private:
    Scene& _scene;
};

#endif // AUTOPILOTSYSTEM_HPP
