#ifndef SAVEGAMESTATE_HPP
#define SAVEGAMESTATE_HPP

#include <vector>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <states/AbstractState.hpp>

// Forward declarations
class SceneSerializer;

class SaveGameState : public AbstractState {
public:
    SaveGameState(StateStack& stack, const SceneSerializer& serializer);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    const SceneSerializer& _serializer;
    tgui::ChildWindow::Ptr _window;
    tgui::Grid::Ptr _grid;
    std::vector<tgui::Widget::Ptr> _savesListWidgets;

    void populateSavesList();
};

#endif // SAVEGAME_HPP
