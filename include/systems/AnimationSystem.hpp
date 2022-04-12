#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct SoundSettings;
struct GameEvent;

class AnimationSystem {
public:
    AnimationSystem(Scene& scene, const SoundSettings& soundSettings);
    void update(sf::Time dt);
    void handleTriggerEvent(const GameEvent& event, bool start);

private:
    Scene& _scene;
    const SoundSettings& _soundSettings;
};

#endif // ANIMATIONSYSTEM_HPP
