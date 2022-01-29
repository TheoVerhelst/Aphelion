#include <stdexcept>
#include <SFML/Audio/Music.hpp>
#include <MusicManager.hpp>
#include <SoundSettings.hpp>

MusicManager::MusicManager(const SoundSettings& soundSettings):
    _soundSettings{soundSettings} {
}

void MusicManager::openFromFile(const std::string& filename) {
    _musics.emplace_back(std::make_shared<sf::Music>());
    if (not _musics.back()->openFromFile(filename)) {
        throw std::runtime_error("Could not load music from " + filename);
    }
    _musics.back()->setVolume(_soundSettings.mainVolume * _soundSettings.musicVolume / 100);
}

void MusicManager::update() {
    if (_musics[_played]->getStatus() == sf::SoundSource::Playing) {
        _musics[_played]->setVolume(_soundSettings.mainVolume * _soundSettings.musicVolume / 100);
    }
    if(not _silence and _musics[_played]->getStatus() == sf::SoundSource::Stopped) {
        // The music has stopped since last update
        _silence = true;
        _silenceClock.restart();
    } else if (_silence and _silenceClock.getElapsedTime() >= _silenceDuration and _musics.size() > 0) {
        // We should play the next music
        _silence = false;
        _played = (_played + 1) % _musics.size();
        _musics[_played]->play();
    }
}
