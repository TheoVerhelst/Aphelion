#ifndef MAINMENUSTATE_HPP
#define MAINMENUSTATE_HPP

#include <states/AbstractState.hpp>

// Forward declarations
namespace tgui {
    class Texture;
}
template <typename T>
class ResourceManager;


class MainMenuState : public AbstractState {
public:
    MainMenuState(StateStack& stack, ResourceManager<tgui::Texture>& tguiTextureManager);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    ResourceManager<tgui::Texture>& _tguiTextureManager;
};

#endif // MAINMENUSTATE_HPP
