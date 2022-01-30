#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <utility>

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct SoundSettings;
enum class Action;
typedef std::pair<Action, bool> TriggerAction;

class AnimationSystem {
public:
    AnimationSystem(Scene& scene, const SoundSettings& soundSettings);
    void update(sf::Time dt);
    bool handleTriggerAction(const TriggerAction& actionPair);

private:
    Scene& _scene;
    const SoundSettings& _soundSettings;
};

#endif // ANIMATIONSYSTEM_HPP
