#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <json.hpp>
#include <Input.hpp>
#include <serializers.hpp>

struct SoundSettings {
    float mainVolume;
    float effectsVolume;
    float musicVolume;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SoundSettings, mainVolume,
    effectsVolume, musicVolume)

struct Settings {
    SoundSettings soundSettings;
    sf::VideoMode videoMode;
    std::map<sf::Keyboard::Key, GameInput> gameKeyboardMapping;
    std::map<ControllerButton, GameInput> gameControllerMapping;
    std::map<sf::Keyboard::Key, MapInput> mapKeyboardMapping;
    std::map<ControllerButton, MapInput> mapControllerMapping;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, soundSettings, videoMode,
    gameKeyboardMapping, gameControllerMapping, mapKeyboardMapping,
    mapControllerMapping)

#endif // SETTINGS_HPP
