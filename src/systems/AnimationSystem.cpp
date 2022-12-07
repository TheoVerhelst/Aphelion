#include <SFML/System/Time.hpp>
#include <systems/AnimationSystem.hpp>
#include <Scene.hpp>
#include <Animation.hpp>
#include <Settings.hpp>
#include <Event.hpp>
#include <components/Animations.hpp>
#include <components/components.hpp>

AnimationSystem::AnimationSystem(Scene& scene, const SoundSettings& soundSettings):
    _scene{scene},
    _soundSettings{soundSettings} {
}

void AnimationSystem::update(sf::Time dt) {
    for (auto& [id, animations] : _scene.view<Animations>()) {
        for (auto& [type, data] : animations) {
            data.animation.setVolume(_soundSettings.mainVolume * _soundSettings.effectsVolume / 100);
            if (not data.animation.isStopped()) {
                data.animation.update(dt);
            }
        }
    }
}

void AnimationSystem::handleEvent(const Event& event) {
    std::optional<AnimationType> type{getAnimationType(event)};
    if (type.has_value()) {
        Animations& animations{_scene.getComponent<Animations>(event.entity)};
        auto animationIt = animations.find(type.value());
        if (animationIt != animations.end()) {
            if (event.start) {
                animationIt->second.animation.start();
            } else {
                animationIt->second.animation.stop();
            }
        }
    }
}

std::optional<AnimationType> AnimationSystem::getAnimationType(const Event& event) {
    if (std::holds_alternative<Event::EngineEvent>(event.data)) {
        return AnimationType::Engine;
    } else if (std::holds_alternative<Event::RcsEvent>(event.data)) {
        switch (std::get<Event::RcsEvent>(event.data)) {
        case Event::RcsEvent::Up:
            return AnimationType::RcsUp;
        case Event::RcsEvent::Down:
            return AnimationType::RcsDown;
        case Event::RcsEvent::Left:
            return AnimationType::RcsLeft;
        case Event::RcsEvent::Right:
            return AnimationType::RcsRight;
        case Event::RcsEvent::Clockwise:
            return AnimationType::RcsClockwise;
        case Event::RcsEvent::CounterClockwise:
            return AnimationType::RcsCounterClockwise;
        default:
            break;
        }
    }
    return {};
}
