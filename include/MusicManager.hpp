#ifndef MUSICMANAGER_HPP
#define MUSICMANAGER_HPP

#include <string>
#include <cstddef>
#include <vector>
#include <memory>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

// Forward declarations
namespace sf {
    class Music;
}
struct SoundSettings;

class MusicManager {
public:
    MusicManager(const SoundSettings& soundSettings);
    void openFromFile(const std::string& filename);
    void update();

private:
    const SoundSettings& _soundSettings;
    std::vector<std::shared_ptr<sf::Music>> _musics;
    sf::Time _silenceDuration{sf::seconds(60)};
    sf::Clock _silenceClock;
    std::size_t _played{0};
    bool _silence{true};
};

#endif // MUSICMANAGER_HPP
