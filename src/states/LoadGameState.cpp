#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <states/StateStack.hpp>
#include <states/GameState.hpp>
#include <states/LoadGameState.hpp>
#include <Paths.hpp>
#include <Action.hpp>

LoadGameState::LoadGameState(StateStack& stack):
    AbstractState{stack} {
}

tgui::Widget::Ptr LoadGameState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Load game")};
    window->setSize("25%", "50%");
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this] {
        _stack.popStatesUntil(*this);
    });

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("5%", "5%");
    window->add(grid);

    // TODO Use tgui::ScrollablePanel instead of Grid
    std::size_t row{0};
    for (auto& path : Paths::getSavePaths()) {
        tgui::Label::Ptr label{tgui::Label::create(std::string(path.stem()))};
        label->setTextSize(20);
        label->setSize(tgui::bindInnerWidth(window) * 0.7, 40);
        grid->addWidget(label, row, 0, tgui::Grid::Alignment::Left);

        tgui::Button::Ptr loadButton{tgui::Button::create("Load")};
        loadButton->onPress([this, path]() noexcept {
            _stack.clearStates();
            _stack.pushState<GameState, const std::filesystem::path&>(path);
        });
        loadButton->setTextSize(20);
        loadButton->setSize(tgui::bindInnerWidth(window) * 0.2, 30);
        grid->addWidget(loadButton, row, 1, tgui::Grid::Alignment::Right, {10, 0});
        row++;
    }

    tgui::Button::Ptr cancelButton{tgui::Button::create("Cancel")};
    cancelButton->onPress([this] {
        _stack.popStatesUntil(*this);
    });
    cancelButton->setTextSize(18);
    cancelButton->setPosition("95%", "95%");
    cancelButton->setOrigin(1.f, 1.f);
    window->add(cancelButton);

    return window;
}

bool LoadGameState::update(sf::Time) {
    return true;
}

bool LoadGameState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::Exit and start) {
        _stack.popStatesUntil(*this);
    }
    return true;
}

bool LoadGameState::handleContinuousAction(const Action&, sf::Time) {
    return true;
}
