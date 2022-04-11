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
    virtual bool handleEvent(const sf::Event& event) override;

private:
    ResourceManager<tgui::Texture>& _tguiTextureManager;
};

#endif // MAINMENUSTATE_HPP
