#ifndef SOUNDEFFECTSSYSTEM_HPP
#define SOUNDEFFECTSSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct SoundSettings;
struct Event;

class SoundEffectsSystem {
public:
    SoundEffectsSystem(Scene& scene, const SoundSettings& soundSettings);
    void handleEvent(const Event& event);

private:
    Scene& _scene;
    const SoundSettings& _soundSettings;
    // Manually tuned value that correspond to a moderate impact, and discards
    // all ground contact collision events when the object is not moving.
    const float _impactThreshold{500000.f};
};

#endif // SOUNDEFFECTSSYSTEM_HPP
