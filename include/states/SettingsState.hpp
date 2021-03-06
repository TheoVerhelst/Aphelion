#ifndef SETTINGSSTATE_HPP
#define SETTINGSSTATE_HPP

#include <map>
#include <string>
#include <SFML/Window/VideoMode.hpp>
#include <states/AbstractState.hpp>
#include <SoundSettings.hpp>
#include <vector.hpp>

namespace sf {
    class Window;
}

class SettingsState : public AbstractState {
public:
    SettingsState(StateStack& stack, SoundSettings& soundSettings, sf::Window& window);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    SoundSettings& _soundSettings;
    const SoundSettings _initialSoundSettings;
    sf::Window& _window;
    Vector2u _initalScreenSize;
    std::map<std::string, sf::VideoMode> _videoModes;
    std::vector<std::string> _videoModeOrder;
};

#endif // SETTINGSSTATE_HPP
