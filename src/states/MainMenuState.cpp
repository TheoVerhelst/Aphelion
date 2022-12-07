#include <SFML/Window/Event.hpp>
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
#include <Paths.hpp>
#include <Input.hpp>

MainMenuState::MainMenuState(StateStack& stack, ResourceManager<tgui::Texture>& tguiTextureManager):
    AbstractState{stack},
    _tguiTextureManager{tguiTextureManager} {
}

tgui::Widget::Ptr MainMenuState::buildGui() {
    tgui::Group::Ptr group{tgui::Group::create()};

    tgui::Picture::Ptr background{tgui::Picture::create(_tguiTextureManager.get("background"))};
    group->add(background);

    tgui::Label::Ptr title{tgui::Label::create("Aphelion")};
    title->setTextSize(120);
    title->setPosition("50%", "15%");
    title->setOrigin(0.5f, 0.5f);
    title->getRenderer()->setTextColor(tgui::Color::White);
    title->getRenderer()->setFont("resources/fonts/Brodo.ttf");
    group->add(title);

    tgui::VerticalLayout::Ptr layout{tgui::VerticalLayout::create()};
    layout->setSize(700, 300);
    layout->setPosition("50%", "95%");
    layout->setOrigin(0.5f, 1.f);
    group->add(layout);


    std::vector<std::pair<std::string, std::function<void()>>> buttons{
        {"New game", [this] {
            _stack.clearStates();
            _stack.pushState<GameState, const std::filesystem::path&>(Paths::getNewGameSavePath());
        }},
        {"Settings", [this] {
            _stack.pushState<SettingsState>();
        }},
        {"Exit game", [this] {
            _stack.clearStates();
        }}
    };

    const std::vector<std::filesystem::path> savePaths{Paths::getSavePaths()};
    if (not savePaths.empty()) {
        buttons.insert(buttons.begin() + 1, {"Continue", [this] {
            _stack.clearStates();
            _stack.pushState<GameState, const std::filesystem::path&>(Paths::getMostRecentSavePath());
        }});
    }

    for (auto& [text, function] : buttons) {
        tgui::Button::Ptr button{tgui::Button::create(text)};
        button->onPress(function);
        button->setTextSize(30);
        layout->add(button);
        layout->addSpace(0.2f);
    }
    // Remove last spacing
    layout->setRatio(2 * buttons.size() - 1, 0.f);

    return group;
}

bool MainMenuState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.clearStates();
        return true;
    }
    return false;
}
