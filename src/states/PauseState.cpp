#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <states/StateStack.hpp>
#include <states/PauseState.hpp>
#include <states/SettingsState.hpp>
#include <Action.hpp>
#include <SoundSettings.hpp>

PauseState::PauseState(StateStack& stack, SoundSettings& soundSettings):
    AbstractState{stack},
    _soundSettings{soundSettings} {
}

tgui::Widget::Ptr PauseState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Pause")};
    window->setSize(200, 300);
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this]{_stack.popState();});

    tgui::VerticalLayout::Ptr layout{tgui::VerticalLayout::create()};
    layout->setSize("80%", "80%");
    layout->setPosition("50%", "50%");
    layout->setOrigin(0.5f, 0.5f);
    window->add(layout);

    tgui::Button::Ptr continueButton{tgui::Button::create("Continue")};
    continueButton->onPress([this]{_stack.popState();});
    continueButton->setTextSize(18);
    layout->add(continueButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr settingsButton{tgui::Button::create("Settings")};
    settingsButton->onPress([this]{_stack.pushState(new SettingsState(_stack, _soundSettings));});
    settingsButton->setTextSize(18);
    layout->add(settingsButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr mainMenuButton{tgui::Button::create("Main menu")};
    mainMenuButton->onPress([this]{_stack.popState();_stack.popState();});
    mainMenuButton->setTextSize(18);
    layout->add(mainMenuButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr exitGameButton{tgui::Button::create("Exit game")};
    exitGameButton->onPress([this]{_stack.popState();_stack.popState();});
    exitGameButton->setTextSize(18);
    layout->add(exitGameButton);

    return window;
}

bool PauseState::update(sf::Time) {
    return true;
}

bool PauseState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::Exit and start) {
        _stack.popState();
    }
    return true;
}

bool PauseState::handleContinuousAction(const Action&, sf::Time) {
    return true;
}
