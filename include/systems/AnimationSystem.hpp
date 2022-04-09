#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <utility>

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct SoundSettings;
enum class GameAction;

class AnimationSystem {
public:
    AnimationSystem(Scene& scene, const SoundSettings& soundSettings);
    void update(sf::Time dt);
    bool handleTriggerAction(const GameAction& action, bool start);

private:
    Scene& _scene;
    const SoundSettings& _soundSettings;
};

#endif // ANIMATIONSYSTEM_HPP
