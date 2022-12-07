#include <SFML/Window/Event.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <states/StateStack.hpp>
#include <states/PauseState.hpp>
#include <states/MainMenuState.hpp>
#include <states/LoadGameState.hpp>
#include <states/SaveGameState.hpp>
#include <states/SettingsState.hpp>
#include <Input.hpp>
#include <SceneSerializer.hpp>

PauseState::PauseState(StateStack& stack, const SceneSerializer& serializer):
    AbstractState{stack},
    _serializer{serializer} {
}

tgui::Widget::Ptr PauseState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Pause")};
    window->setSize(200, 350);
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this] {
        _stack.popStatesUntil(*this);
    });
    // TODO focus the window

    tgui::VerticalLayout::Ptr layout{tgui::VerticalLayout::create()};
    layout->setSize("80%", "80%");
    layout->setPosition("50%", "50%");
    layout->setOrigin(0.5f, 0.5f);
    window->add(layout);

    tgui::Button::Ptr continueButton{tgui::Button::create("Continue")};
    continueButton->onPress([this]{
        _stack.popStatesUntil(*this);
    });
    continueButton->setTextSize(18);
    layout->add(continueButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr saveButton{tgui::Button::create("Save game")};
    saveButton->onPress([this] {
        _stack.pushState<SaveGameState>(_serializer);
    });
    saveButton->setTextSize(18);
    layout->add(saveButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr loadButton{tgui::Button::create("Load game")};
    loadButton->onPress([this] {
        _stack.pushState<LoadGameState>();
    });
    loadButton->setTextSize(18);
    layout->add(loadButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr settingsButton{tgui::Button::create("Settings")};
    settingsButton->onPress([this] {
        _stack.pushState<SettingsState>();
    });
    settingsButton->setTextSize(18);
    layout->add(settingsButton);
    layout->addSpace(0.1f);

    tgui::Button::Ptr mainMenuButton{tgui::Button::create("Main menu")};
    mainMenuButton->onPress([this]{
        _stack.clearStates();
        _stack.pushState<MainMenuState>();
    });
    mainMenuButton->setTextSize(18);
    layout->add(mainMenuButton);
    layout->addSpace(0.1f);

    return window;
}

bool PauseState::update(sf::Time) {
    // Pause the game
    return true;
}

bool PauseState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.popStatesUntil(*this);
    }
    // Block inputs
    return true;
}

bool PauseState::handleContinuousInputs(sf::Time) {
    // Block inputs
    return true;
}
