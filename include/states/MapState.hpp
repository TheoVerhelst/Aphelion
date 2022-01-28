#ifndef MAPSTATE_HPP
#define MAPSTATE_HPP

#include <TGUI/Widgets/Group.hpp>
#include <states/AbstractState.hpp>
#include <vector.hpp>

// Forward declarations
class StateStack;
class Scene;

class MapState : public AbstractState {
public:
    MapState(StateStack& stack, Scene& scene);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual void update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    Scene& _scene;
    tgui::Group::Ptr _mapIcons{tgui::Group::create()};
    const float _zoomSpeed{15};
    float _scale{50};
    const float _minScale{25};
    const float _maxScale{200};
};

#endif // MAPSTATE_HPP
