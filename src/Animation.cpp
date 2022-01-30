#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Animation.hpp>

Animation::Animation(const sf::Texture& texture, const std::vector<AnimationFrame>& frames,
        const sf::SoundBuffer& soundBuffer, sf::Time soundLoopStart, sf::Time soundLoopEnd):
    _frames{frames},
    _sprite{texture, frames.at(0).rect},
    _sound{soundBuffer},
    _soundLoopStart{soundLoopStart},
    _soundLoopEnd{soundLoopEnd} {
    for (auto& frame : _frames) {
        _totalDuration += frame.duration;
    }
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
    _sound.setPlayingOffset(_soundLoopEnd);
    _sound.setLoop(false);
}

bool Animation::isStopped() const {
    return _stopped;
}

sf::Sprite& Animation::getSprite() {
    return _sprite;
}

void Animation::update(sf::Time dt) {
    if (not _stopped) {
        // Loop back the sound if needed
        if (_sound.getStatus() == sf::SoundSource::Playing
                and _sound.getLoop() and _sound.getPlayingOffset() > _soundLoopEnd) {
            _sound.setPlayingOffset(_soundLoopStart);
        }
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

void Animation::setVolume(float volume) {
    _sound.setVolume(volume);
}
