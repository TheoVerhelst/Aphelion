#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <states/StateStack.hpp>
#include <states/SaveGameState.hpp>
#include <Action.hpp>
#include <SceneSerializer.hpp>
#include <Paths.hpp>

SaveGameState::SaveGameState(StateStack& stack, const SceneSerializer& serializer):
    AbstractState{stack},
    _serializer{serializer} {
}

tgui::Widget::Ptr SaveGameState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Save game")};
    window->setSize("25%", "50%");
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this] {
        _stack.popStatesUntil(*this);
    });

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("5%", "5%");
    window->add(grid);

    tgui::EditBox::Ptr newSaveEdit{tgui::EditBox::create()};
    newSaveEdit->setDefaultText("New save...");
    newSaveEdit->setSize(tgui::bindInnerWidth(window) * 0.7, 40);
    grid->addWidget(newSaveEdit, 0, 0, tgui::Grid::Alignment::Left);

    tgui::Button::Ptr newSaveButton{tgui::Button::create("Save")};
    newSaveButton->onPress([this, newSaveButton, newSaveEdit]() noexcept {
        const std::string stem{newSaveEdit->getText()};
        if (not stem.empty()) {
            newSaveButton->setText("Saved!");
            _serializer.save(Paths::savePathFromStem(stem));
        }
    });
    newSaveButton->setTextSize(20);
    newSaveButton->setSize(tgui::bindInnerWidth(window) * 0.2, 30);
    grid->addWidget(newSaveButton, 0, 1, tgui::Grid::Alignment::Right, {10, 0});

    // TODO Use tgui::ScrollablePanel instead of Grid
    std::size_t row{1};
    for (auto& path : Paths::getSavePaths()) {
        tgui::Label::Ptr label{tgui::Label::create(std::string(path.stem()))};
        label->setTextSize(20);
        label->setSize(tgui::bindInnerWidth(window) * 0.7, 40);
        grid->addWidget(label, row, 0, tgui::Grid::Alignment::Left);

        tgui::Button::Ptr saveButton{tgui::Button::create("Save")};
        saveButton->onPress([this, path, saveButton]() noexcept {
            saveButton->setText("Saved!");
            _serializer.save(path);
        });
        saveButton->setTextSize(20);
        saveButton->setSize(tgui::bindInnerWidth(window) * 0.2, 30);
        grid->addWidget(saveButton, row, 1, tgui::Grid::Alignment::Right, {10, 0});
        row++;
    }

    tgui::Button::Ptr okButton{tgui::Button::create("OK")};
    okButton->onPress([this] {
        _stack.popStatesUntil(*this);
    });
    okButton->setTextSize(18);
    okButton->setPosition("95%", "95%");
    okButton->setOrigin(1.f, 1.f);
    window->add(okButton);

    return window;
}

bool SaveGameState::update(sf::Time) {
    return true;
}

bool SaveGameState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::Exit and start) {
        _stack.popStatesUntil(*this);
    }
    return true;
}

bool SaveGameState::handleContinuousAction(const Action&, sf::Time) {
    return true;
}
