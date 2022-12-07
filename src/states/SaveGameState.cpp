#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <states/StateStack.hpp>
#include <states/SaveGameState.hpp>
#include <Input.hpp>
#include <SceneSerializer.hpp>
#include <Paths.hpp>

SaveGameState::SaveGameState(StateStack& stack, const SceneSerializer& serializer):
    AbstractState{stack},
    _serializer{serializer} {
}

tgui::Widget::Ptr SaveGameState::buildGui() {
    _panel = tgui::Panel::create({"25%", "100%"});
    _panel->setPosition("25%", "0%");

    _grid = tgui::Grid::create();
    _grid->setPosition("5%", "5%");
    _panel->add(_grid);

    tgui::EditBox::Ptr newSaveEdit{tgui::EditBox::create()};
    newSaveEdit->setDefaultText("New save...");
    newSaveEdit->setSize(tgui::bindInnerWidth(_panel) * 0.7, 40);
    _grid->addWidget(newSaveEdit, 0, 0, tgui::Grid::Alignment::Left);

    tgui::Button::Ptr newSaveButton{tgui::Button::create("Save")};
    newSaveButton->onPress([this, newSaveButton, newSaveEdit]() noexcept {
        std::string stem{newSaveEdit->getText()};
        if (stem.empty()) {
            stem = Paths::generateStem();
        }
        newSaveButton->setText("Saved!");
        _serializer.save(Paths::savePathFromStem(stem));
        populateSavesList();
    });
    newSaveButton->setTextSize(20);
    newSaveButton->setSize(tgui::bindInnerWidth(_panel) * 0.2, 30);
    _grid->addWidget(newSaveButton, 0, 1, tgui::Grid::Alignment::Right, {10, 0});

    populateSavesList();

    tgui::Button::Ptr okButton{tgui::Button::create("OK")};
    okButton->onPress([this] {
        _stack.popStatesUntil(*this);
    });
    okButton->setTextSize(26);
    okButton->setPosition("50%", "95%");
    okButton->setOrigin(0.5f, 1.f);
    _panel->add(okButton);

    return _panel;
}

bool SaveGameState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.popStatesUntil(*this);
        return true;
    }
    return false;
}

void SaveGameState::populateSavesList() {
    // Clear the current list
    for (auto& widget : _savesListWidgets) {
        _grid->remove(widget);
    }
    _savesListWidgets.clear();

    // Populate the list with the save files
    // TODO Use tgui::ScrollablePanel instead of Grid
    std::size_t row{1};
    for (auto& path : Paths::getSavePaths()) {
        tgui::Label::Ptr label{tgui::Label::create(std::string(path.stem()))};
        label->setSize(tgui::bindInnerWidth(_panel) * 0.7, 40);
        _grid->addWidget(label, row, 0, tgui::Grid::Alignment::Left);
        _savesListWidgets.push_back(label);

        tgui::Button::Ptr saveButton{tgui::Button::create("Save")};
        saveButton->onPress([this, path, saveButton]() noexcept {
            saveButton->setText("Saved!");
            _serializer.save(path);
        });
        saveButton->setTextSize(20);
        saveButton->setSize(tgui::bindInnerWidth(_panel) * 0.2, 30);
        _grid->addWidget(saveButton, row, 1, tgui::Grid::Alignment::Right, {10, 0});
        _savesListWidgets.push_back(saveButton);
        row++;
    }
}
