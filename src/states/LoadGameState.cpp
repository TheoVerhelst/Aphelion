#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <states/StateStack.hpp>
#include <states/GameState.hpp>
#include <states/LoadGameState.hpp>
#include <Paths.hpp>
#include <Input.hpp>

LoadGameState::LoadGameState(StateStack& stack):
    AbstractState{stack} {
}

tgui::Widget::Ptr LoadGameState::buildGui() {
    float labelHeight{34};
    float widgetHeight{32};
    unsigned int labelTextSize{14};

    tgui::Panel::Ptr panel{tgui::Panel::create({"50%", "100%"})};
    panel->setPosition("25%", "0%");

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("5%", "5%");
    panel->add(grid);

    // TODO Use tgui::ScrollablePanel instead of Grid
    std::size_t row{0};
    for (auto& path : Paths::getSavePaths()) {
        tgui::Label::Ptr label{tgui::Label::create(std::string(path.stem()))};
        label->setSize(tgui::bindInnerWidth(panel) * 0.7, labelHeight);
        label->setTextSize(labelTextSize);
        label->setVerticalAlignment(tgui::VerticalAlignment::Center);
        grid->addWidget(label, row, 0, tgui::Grid::Alignment::Left);

        tgui::Button::Ptr loadButton{tgui::Button::create("Load")};
        loadButton->onPress([this, path]() noexcept {
            _stack.clearStates();
            _stack.pushState<GameState, const std::filesystem::path&>(path);
        });
        loadButton->setTextSize(labelTextSize);
        loadButton->setSize(tgui::bindInnerWidth(panel) * 0.2, widgetHeight);
        grid->addWidget(loadButton, row, 1, tgui::Grid::Alignment::Right, {10, 0});
        row++;
    }
    return panel;
}

bool LoadGameState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.popStatesUntil(*this);
        return true;
    }
    return false;
}
