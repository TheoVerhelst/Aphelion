#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <map>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <json.hpp>
#include <Input.hpp>
#include <serializers.hpp>

struct SoundSettings {
    float mainVolume{100};
    float effectsVolume{50};
    float musicVolume{70};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SoundSettings, mainVolume,
    effectsVolume, musicVolume)

struct Settings {
    SoundSettings soundSettings;
    sf::VideoMode videoMode{sf::VideoMode::getDesktopMode()};
    std::map<sf::Keyboard::Key, GameInput> gameKeyboardMapping {
        {sf::Keyboard::Z, GameInput::RcsUp},
        {sf::Keyboard::Q, GameInput::RcsLeft},
        {sf::Keyboard::S, GameInput::RcsDown},
        {sf::Keyboard::D, GameInput::RcsRight},
        {sf::Keyboard::A, GameInput::RcsCounterClockwise},
        {sf::Keyboard::E, GameInput::RcsClockwise},
        {sf::Keyboard::Space, GameInput::Engine},
        {sf::Keyboard::LShift, GameInput::ZoomIn},
        {sf::Keyboard::RShift, GameInput::ZoomIn},
        {sf::Keyboard::LControl, GameInput::ZoomOut},
        {sf::Keyboard::RControl, GameInput::ZoomOut},
        {sf::Keyboard::M, GameInput::ToggleMap},
        {sf::Keyboard::LAlt, GameInput::RotateView},
        {sf::Keyboard::Escape, GameInput::Pause}
    };
    std::map<ControllerButton, GameInput> gameControllerMapping {
        {ControllerButton::X, GameInput::Engine}
    };
    std::map<sf::Keyboard::Key, MapInput> mapKeyboardMapping {
        {sf::Keyboard::LShift, MapInput::ZoomIn},
        {sf::Keyboard::RShift, MapInput::ZoomIn},
        {sf::Keyboard::LControl, MapInput::ZoomOut},
        {sf::Keyboard::RControl, MapInput::ZoomOut},
        {sf::Keyboard::M, MapInput::Exit},
        {sf::Keyboard::Escape, MapInput::Exit}
    };
    std::map<ControllerButton, MapInput> mapControllerMapping {
        {ControllerButton::X, MapInput::ZoomIn}
    };

    // Loads settings from settings.json if present, and defaults to
    // default-constructed members otherwise.
    static Settings loadSettings();

    // Write the settings to settings.json
    static void saveSettings(const Settings& settings);
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, soundSettings, videoMode,
    gameKeyboardMapping, gameControllerMapping, mapKeyboardMapping,
    mapControllerMapping)

#endif // SETTINGS_HPP
