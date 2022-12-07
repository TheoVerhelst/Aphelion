#include <SFML/Window/Event.hpp>
#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/Panel.hpp>
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
    tgui::Panel::Ptr panel{tgui::Panel::create({"25%", "100%"})};
    panel->setPosition(0, 0);

    tgui::VerticalLayout::Ptr layout{tgui::VerticalLayout::create()};
    layout->setSize("80%", "70%");
    layout->setPosition("10%", "15%");
    panel->add(layout);

    std::vector<std::pair<std::string, std::function<void()>>> buttons{
        {"Continue", [this] {
            _stack.popStatesUntil(*this);
        }},
        {"Save game", [this] {
            _stack.popStatesUpTo(*this);
            _stack.pushState<SaveGameState>(_serializer);
        }},
        {"Load game", [this] {
            _stack.popStatesUpTo(*this);
            _stack.pushState<LoadGameState>();
        }},
        {"Settings", [this] {
            _stack.popStatesUpTo(*this);
            _stack.pushState<SettingsState>();
        }},
        {"Main menu", [this] {
            _stack.clearStates();
            _stack.pushState<MainMenuState>();
        }}
    };

    for (auto& [text, function] : buttons) {
        tgui::Button::Ptr button{tgui::Button::create(text)};
        button->onPress(function);
        button->setTextSize(26);
        layout->add(button);
        layout->addSpace(0.15f);
    }

    return panel;
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
