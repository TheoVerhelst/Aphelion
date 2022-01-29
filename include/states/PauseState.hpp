#ifndef PAUSESTATE_HPP
#define PAUSESTATE_HPP

#include <states/AbstractState.hpp>

// Forward declarations
struct SoundSettings;


class PauseState : public AbstractState {
public:
    PauseState(StateStack& stack, SoundSettings& soundSettings);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    SoundSettings& _soundSettings;
};

#endif // PAUSESTATE_HPP
