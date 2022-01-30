#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <states/StateStack.hpp>
#include <states/PauseState.hpp>
#include <states/MainMenuState.hpp>
#include <states/LoadGameState.hpp>
#include <states/SettingsState.hpp>
#include <Action.hpp>
#include <SoundSettings.hpp>

PauseState::PauseState(StateStack& stack):
    AbstractState{stack} {
}

tgui::Widget::Ptr PauseState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Pause")};
    window->setSize(200, 300);
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this]{_stack.popState();});
    // TODO focus the window

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

    tgui::Button::Ptr saveButton{tgui::Button::create("Save game")};
    saveButton->onPress([this]{_stack.pushState<LoadGameState>();});
    saveButton->setTextSize(18);
    layout->add(saveButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr loadButton{tgui::Button::create("Load game")};
    loadButton->onPress([this]{_stack.pushState<LoadGameState>();});
    loadButton->setTextSize(18);
    layout->add(loadButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr settingsButton{tgui::Button::create("Settings")};
    settingsButton->onPress([this]{_stack.pushState<SettingsState>();});
    settingsButton->setTextSize(18);
    layout->add(settingsButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr mainMenuButton{tgui::Button::create("Main menu")};
    mainMenuButton->onPress([this]{
        _stack.popState();
        _stack.popState();
        _stack.pushState<MainMenuState>();
    });
    mainMenuButton->setTextSize(18);
    layout->add(mainMenuButton);
    layout->addSpace(0.1f);

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
