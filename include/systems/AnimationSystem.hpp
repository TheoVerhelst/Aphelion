#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <optional>

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct SoundSettings;
struct Event;
enum class AnimationType;

class AnimationSystem {
public:
    AnimationSystem(Scene& scene, const SoundSettings& soundSettings);
    void update(sf::Time dt);
    void handleEvent(const Event& event);

private:
    Scene& _scene;
    const SoundSettings& _soundSettings;

    static std::optional<AnimationType> getAnimationType(const Event& event);
};

#endif // ANIMATIONSYSTEM_HPP
