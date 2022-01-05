#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <Action.hpp>

struct AnimationFrame {
    sf::Time duration;
    sf::IntRect rect;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationFrame, duration, rect)

class Animation : public sf::Drawable {
public:
    Animation(const std::weak_ptr<const sf::Texture>& texture, const std::vector<AnimationFrame>& frames);
    void start();
    void stop();
    bool isStopped() const;
    sf::Sprite& getSprite();
    void update(const sf::Time& dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    std::vector<AnimationFrame> _frames;
    sf::Sprite _sprite;
    sf::Time _elapsedTime;
    sf::Time _totalDuration;
    bool _stopped{true};
};

typedef std::map<Action, Animation> AnimationComponent;

#endif // ANIMATION_HPP
