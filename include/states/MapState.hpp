#ifndef MAPSTATE_HPP
#define MAPSTATE_HPP

#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <states/AbstractState.hpp>
#include <vector.hpp>
#include <InputManager.hpp>
#include <Input.hpp>

// Forward declarations
namespace tgui {
    class Texture;
}
template <typename T>
class ResourceManager;
class Scene;

class MapState : public AbstractState {
public:
    MapState(StateStack& stack, ResourceManager<tgui::Texture>& tguiTextureManager, Scene& scene);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    virtual bool handleContinuousInputs(sf::Time dt) override;

private:
    ResourceManager<tgui::Texture>& _tguiTextureManager;
    Scene& _scene;
    InputManager<MapInput> _inputManager;
    tgui::Group::Ptr _mapIcons{tgui::Group::create()};
    tgui::Picture::Ptr _background;
    const float _zoomSpeed{15};
    float _scale{50};
    const float _minScale{25};
    const float _maxScale{200};
};

#endif // MAPSTATE_HPP
