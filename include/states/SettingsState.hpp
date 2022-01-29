#ifndef SETTINGSSTATE_HPP
#define SETTINGSSTATE_HPP

#include <states/AbstractState.hpp>
#include <SoundSettings.hpp>

class SettingsState : public AbstractState {
public:
    SettingsState(StateStack& stack, SoundSettings& soundSettings);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    SoundSettings& _soundSettings;
    const SoundSettings _initialSoundSettings;
};

#endif // SETTINGSSTATE_HPP
