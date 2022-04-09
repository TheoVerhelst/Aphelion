#ifndef SAVEGAMESTATE_HPP
#define SAVEGAMESTATE_HPP

#include <states/AbstractState.hpp>

// Forward declarations
class SceneSerializer;

class SaveGameState : public AbstractState {
public:
    SaveGameState(StateStack& stack, const SceneSerializer& serializer);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;

private:
    const SceneSerializer& _serializer;
};

#endif // SAVEGAME_HPP
