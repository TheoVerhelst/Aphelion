#include <LoopedSound.hpp>

LoopedSound::LoopedSound(const sf::SoundBuffer& buffer, sf::Time loopStart, sf::Time loopEnd):
    sf::Sound{buffer},
    _loopStart{loopStart},
    _loopEnd{loopEnd} {
}

void LoopedSound::setLoopStart(sf::Time loopStart) {
    _loopStart = loopStart;
}

void LoopedSound::setLoopEnd(sf::Time loopEnd) {
    _loopEnd = loopEnd;
}

void LoopedSound::stopLoop() {
    setPlayingOffset(_loopEnd);
    setLoop(false);
}

void LoopedSound::update() {
    if (getStatus() == sf::SoundSource::Playing
            and getLoop() and getPlayingOffset() > _loopEnd) {
        setPlayingOffset(_loopStart);
    }
}
