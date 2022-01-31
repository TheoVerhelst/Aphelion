#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <states/StateStack.hpp>
#include <states/GameState.hpp>
#include <states/LoadGameState.hpp>
#include <states/MainMenuState.hpp>
#include <states/SettingsState.hpp>
#include <ResourceManager.hpp>
#include <Action.hpp>

MainMenuState::MainMenuState(StateStack& stack, const ResourceManager<tgui::Texture>& tguiTextureManager):
    AbstractState{stack},
    _tguiTextureManager{tguiTextureManager} {
}

tgui::Widget::Ptr MainMenuState::buildGui() {
    tgui::Group::Ptr group{tgui::Group::create()};

    tgui::Picture::Ptr background{tgui::Picture::create(_tguiTextureManager.get("background"))};
    group->add(background);

    tgui::Label::Ptr title{tgui::Label::create("Aphelion")};
    title->setTextSize(120);
    title->setPosition("50%", "25%");
    title->setOrigin(0.5f, 0.5f);
    title->getRenderer()->setTextColor(tgui::Color::White);
    title->getRenderer()->setFont("resources/fonts/Brodo.ttf");
    group->add(title);

    tgui::VerticalLayout::Ptr layout{tgui::VerticalLayout::create()};
    layout->setSize("50%", "50%");
    layout->setPosition("50%", "70%");
    layout->setOrigin(0.5f, 0.5f);
    group->add(layout);

    tgui::Button::Ptr newGameButton{tgui::Button::create("New game")};
    newGameButton->onPress([this] {
        _stack.clearStates();
        _stack.pushState<GameState, const std::filesystem::path&>("saves/save.json");
    });
    newGameButton->setTextSize(30);
    layout->add(newGameButton);
    layout->addSpace(0.2f);

    tgui::Button::Ptr continueButton{tgui::Button::create("Continue")};
    continueButton->onPress([this]{
        _stack.pushState<LoadGameState>();
    });
    continueButton->setTextSize(30);
    layout->add(continueButton);
    layout->addSpace(0.2f);

    tgui::Button::Ptr settingsButton{tgui::Button::create("Settings")};
    settingsButton->onPress([this]{
        _stack.pushState<SettingsState>();
    });
    settingsButton->setTextSize(30);
    layout->add(settingsButton);
    layout->addSpace(0.2f);

    tgui::Button::Ptr exitGameButton{tgui::Button::create("Exit game")};
    exitGameButton->onPress([this]{
        _stack.clearStates();
    });
    exitGameButton->setTextSize(30);
    layout->add(exitGameButton);

    return group;
}

bool MainMenuState::update(sf::Time) {
    return true;
}

bool MainMenuState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::Exit and start) {
        _stack.clearStates();
    }
    return true;
}

bool MainMenuState::handleContinuousAction(const Action&, sf::Time) {
    return true;
}
