#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Time.hpp>
#include <json.hpp>
#include <serializers.hpp>

struct AnimationFrame {
    sf::Time duration;
    sf::IntRect rect;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationFrame, duration, rect)

class Animation {
public:
    Animation() = default;
    Animation(const sf::Texture& texture, const std::vector<AnimationFrame>& frames,
            const sf::SoundBuffer& soundBuffer, sf::Time soundLoopStart, sf::Time soundLoopEnd);
    void update(sf::Time dt);
    void start();
    void stop();
    bool isStopped() const;
    void setVolume(float volume);
    sf::Sprite& getSprite();

private:
    std::vector<AnimationFrame> _frames;
    sf::Sprite _sprite;
    sf::Time _totalDuration;

    sf::Sound _sound;
    sf::Time _soundLoopStart;
    sf::Time _soundLoopEnd;

    sf::Time _elapsedTime;
    bool _stopped{true};
};

#endif // ANIMATION_HPP
