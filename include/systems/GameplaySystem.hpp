#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
enum class Action;
class Scene;


class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    bool handleContinuousAction(const Action& action, sf::Time dt);

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
