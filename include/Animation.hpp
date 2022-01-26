#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <LoopedSound.hpp>

struct AnimationFrame {
    sf::Time duration;
    sf::IntRect rect;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationFrame, duration, rect)

class Animation : public sf::Drawable {
public:
    Animation(const sf::Texture& texture, const std::vector<AnimationFrame>& frames,
            const sf::SoundBuffer& soundBuffer, sf::Time loopStart, sf::Time loopEnd);
    void start();
    void stop();
    bool isStopped() const;
    sf::Sprite& getSprite();
    void update(const sf::Time& dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<AnimationFrame> _frames;
    sf::Sprite _sprite;
    LoopedSound _sound;
    sf::Time _elapsedTime;
    sf::Time _totalDuration;
    bool _stopped{true};
};

#endif // ANIMATION_HPP
