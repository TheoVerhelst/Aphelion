#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
struct GameEvent;
class Scene;


class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    void handleGameEvent(const GameEvent& event, sf::Time dt);

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
