#include <Animation.hpp>

Animation::Animation(const std::weak_ptr<const sf::Texture>& texture, const std::vector<AnimationFrame>& frames):
    _frames{frames},
    _sprite{*texture.lock(), frames.at(0).rect} {
    for (auto& frame : _frames) {
        _totalDuration += frame.duration;
    }
}

void Animation::start() {
    _stopped = false;
    _elapsedTime = sf::Time::Zero;
}

void Animation::stop() {
    _stopped = true;
    _elapsedTime = sf::Time::Zero;
}

bool Animation::isStopped() const {
    return _stopped;
}

sf::Sprite& Animation::getSprite() {
    return _sprite;
}

void Animation::update(const sf::Time& dt) {
    if (not _stopped) {
        // Update the animation timer
        _elapsedTime += dt;
        while (_elapsedTime > _totalDuration) {
            _elapsedTime -= _totalDuration;
        }
        // Find which frame should be displayed
        // We could avoid calls to setTextureRect by remembering it, but I'm
        // not sure this is a real performance issue
        sf::Time frameTime;
        for (auto& frame : _frames) {
            if (_elapsedTime > frameTime and _elapsedTime <= frameTime + frame.duration) {
                _sprite.setTextureRect(frame.rect);
                break;
            }
        }
    }
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (not _stopped) {
        target.draw(_sprite, states);
    }
}
