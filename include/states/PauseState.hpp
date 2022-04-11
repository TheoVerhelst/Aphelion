#ifndef PAUSESTATE_HPP
#define PAUSESTATE_HPP

#include <states/AbstractState.hpp>

// Forward declarations
class SceneSerializer;

class PauseState : public AbstractState {
public:
    PauseState(StateStack& stack, const SceneSerializer& serializer);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    virtual bool handleContinuousInputs(sf::Time dt) override;

private:
    const SceneSerializer& _serializer;
};

#endif // PAUSESTATE_HPP
