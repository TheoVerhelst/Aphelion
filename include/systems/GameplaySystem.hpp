#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <utility>
#include <Observer.hpp>

// Forward declarations
namespace sf {
    class Time;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;
typedef std::pair<Action, sf::Time> ContinuousAction;
class Scene;


class GameplaySystem : public Observer<const TriggerAction&>, public Observer<const ContinuousAction&> {
public:
    GameplaySystem(Scene& scene);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const ContinuousAction& action) override;

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
