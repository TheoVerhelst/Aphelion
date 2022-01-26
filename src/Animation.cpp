#include <SFML/Graphics/RenderTarget.hpp>
#include <Animation.hpp>

Animation::Animation(const sf::Texture& texture, const std::vector<AnimationFrame>& frames,
        const sf::SoundBuffer& soundBuffer, sf::Time loopStart, sf::Time loopEnd):
    _frames{frames},
    _sprite{texture, frames.at(0).rect},
    _sound{soundBuffer, loopStart, loopEnd} {
    for (auto& frame : _frames) {
        _totalDuration += frame.duration;
    }
    _sound.setVolume(50);
}

void Animation::start() {
    _stopped = false;
    _elapsedTime = sf::Time::Zero;
    _sound.play();
    _sound.setLoop(true);
}

void Animation::stop() {
    _stopped = true;
    _elapsedTime = sf::Time::Zero;
    // We don't stop the sound, instead we allow it to finish playing until the
    // end
    _sound.stopLoop();
}

bool Animation::isStopped() const {
    return _stopped;
}

sf::Sprite& Animation::getSprite() {
    return _sprite;
}

void Animation::update(const sf::Time& dt) {
    _sound.update();
    if (not _stopped) {
        // Update the animation timer
        _elapsedTime += dt;
        while (_elapsedTime > _totalDuration) {
            _elapsedTime -= _totalDuration;
        }
        // Find which frame should be displayed
        // We could avoid calls to setTextureRect by remembering it, but I don't
        // think this is a real performance issue
        sf::Time frameTime;
        for (auto& frame : _frames) {
            if (_elapsedTime > frameTime and _elapsedTime <= frameTime + frame.duration) {
                _sprite.setTextureRect(frame.rect);
                break;
            }
            frameTime += frame.duration;
        }
    }
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (not _stopped) {
        target.draw(_sprite, states);
    }
}
