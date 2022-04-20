#include <SFML/System/Time.hpp>
#include <systems/SoundEffectsSystem.hpp>
#include <Scene.hpp>
#include <Animation.hpp>
#include <SoundSettings.hpp>
#include <Event.hpp>
#include <components/Animations.hpp>
#include <components/components.hpp>

SoundEffectsSystem::SoundEffectsSystem(Scene& scene, const SoundSettings& soundSettings):
    _scene{scene},
    _soundSettings{soundSettings} {
}

void SoundEffectsSystem::handleEvent(const Event& event) {
    if (std::holds_alternative<Event::CollisionEvent>(event.data) and
            std::get<Event::CollisionEvent>(event.data).impactStrength > _impactThreshold) {
        SoundEffects& soundEffects{_scene.getComponent<SoundEffects>(event.entity)};
        auto it = soundEffects.find(SoundEffectType::Collision);
        if (it != soundEffects.end()) {
            sf::Sound& sound{it->second.sound};
            sound.setVolume(_soundSettings.mainVolume * _soundSettings.effectsVolume / 100);
            sound.stop();
            sound.play();
        }
    }
}
