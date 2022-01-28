#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <utility>

// Forward declarations
namespace sf {
    class Time;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;
class Scene;


class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    bool handleTriggerAction(const TriggerAction& action);
    bool handleContinuousAction(const Action& action, sf::Time dt);

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
