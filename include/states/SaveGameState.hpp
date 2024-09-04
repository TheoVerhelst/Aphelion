#ifndef SAVEGAMESTATE_HPP
#define SAVEGAMESTATE_HPP

#include <vector>
#include <TGUI/Widgets/Panel.hpp>
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
    tgui::Panel::Ptr _panel;
    tgui::Grid::Ptr _grid;
    std::vector<tgui::Widget::Ptr> _savesListWidgets;
    const float _labelHeight{34};
    const float _widgetHeight{32};
    const unsigned int _labelTextSize{14};

    void populateSavesList();
};

#endif // SAVEGAME_HPP
