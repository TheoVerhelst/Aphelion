#ifndef LOOPEDSOUND_HPP
#define LOOPEDSOUND_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Audio/Sound.hpp>

class LoopedSound : public sf::Sound {
public:
    LoopedSound(const sf::SoundBuffer& buffer, sf::Time loopStart, sf::Time loopEnd);
    void setLoopStart(sf::Time loopStart);
    void setLoopEnd(sf::Time loopEnd);
    void stopLoop();
    void update();

private:
    sf::Time _loopStart;
    sf::Time _loopEnd;
};

#endif // LOOPEDSOUND_HPP
