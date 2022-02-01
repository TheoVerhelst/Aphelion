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
    virtual bool update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;


private:
    SoundSettings& _soundSettings;
    const SoundSettings _initialSoundSettings;
    sf::Window& _window;
    Vector2u _initalScreenSize;
    std::map<std::string, sf::VideoMode> _videoModes;
    std::vector<std::string> _videoModeOrder;
};

#endif // SETTINGSSTATE_HPP
