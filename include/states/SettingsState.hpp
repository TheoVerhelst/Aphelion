#ifndef SETTINGSSTATE_HPP
#define SETTINGSSTATE_HPP

#include <map>
#include <string>
#include <SFML/Window/VideoMode.hpp>
#include <states/AbstractState.hpp>
#include <Settings.hpp>
#include <vector.hpp>

namespace sf {
    class Window;
}

class SettingsState : public AbstractState {
public:
    SettingsState(StateStack& stack, Settings& settings, sf::Window& window);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    Settings& _settings;
    const Settings _initialSettings;
    sf::Window& _window;
    Vector2u _initalScreenSize;
    std::map<std::string, sf::VideoMode> _videoModes;
    std::vector<std::string> _videoModeOrder;

    static std::string videoModeToName(const sf::VideoMode& mode);
};

#endif // SETTINGSSTATE_HPP
