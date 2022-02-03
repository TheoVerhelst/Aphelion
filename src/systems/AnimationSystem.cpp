#include <SFML/System/Time.hpp>
#include <systems/AnimationSystem.hpp>
#include <Scene.hpp>
#include <Animation.hpp>
#include <SoundSettings.hpp>
#include <Action.hpp>
#include <components/Animations.hpp>
#include <components/components.hpp>

AnimationSystem::AnimationSystem(Scene& scene, const SoundSettings& soundSettings):
    _scene{scene},
    _soundSettings{soundSettings} {
}

void AnimationSystem::update(sf::Time dt) {
    for (EntityId id : _scene.view<Animations>()) {
        Animations& animations{_scene.getComponent<Animations>(id)};
        for (auto& [action, animationData] : animations) {
            animationData.animation.setVolume(_soundSettings.mainVolume * _soundSettings.effectsVolume / 100);
            if (not animationData.animation.isStopped()) {
                animationData.animation.update(dt);
            }
        }
    }
}

bool AnimationSystem::handleTriggerAction(const TriggerAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, start] = actionPair;

    // Play the player animations
    Animations& animations{_scene.getComponent<Animations>(playerId)};
    auto animationIt = animations.find(action);
    if (animationIt != animations.end()) {
        if (start) {
            animationIt->second.animation.start();
        } else {
            animationIt->second.animation.stop();
        }
        return true;
    }
    return false;
}
