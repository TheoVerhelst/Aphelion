#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Texture.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <Scene.hpp>
#include <ResourceManager.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>
#include <Action.hpp>

MapState::MapState(StateStack& stack, ResourceManager<tgui::Texture>& tguiTextureManager, Scene& scene):
    AbstractState{stack},
    _tguiTextureManager{tguiTextureManager},
    _scene{scene},
    _background{tgui::Picture::create(_tguiTextureManager.get("mapBackground"))} {
}

tgui::Widget::Ptr MapState::buildGui() {
    for (EntityId id : _scene.view<MapElement>()) {
        _mapIcons->add(_scene.getComponent<MapElement>(id).icon);
    }
    tgui::Group::Ptr group{tgui::Group::create()};
    group->add(_background);
    group->add(_mapIcons);
    return group;
}

bool MapState::update(sf::Time) {
    const Body& playerBody{_scene.getComponent<Body>(_scene.findUnique<Player>())};
    const Vector2f playerPos{playerBody.position};
    const Vector2f mapSize{_mapIcons->getSize()};

    for (EntityId id : _scene.view<Body, MapElement>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        MapElement& mapElement{_scene.getComponent<MapElement>(id)};
        // Compute the position of the map element on the screen. Note that we
        // don't rotate the map icon.
        Vector2f screenPos{(body.position - playerPos) / _scale};
        screenPos += mapSize / 2.f;
        mapElement.icon->setPosition(static_cast<tgui::Vector2f>(screenPos));
        // Except for ship icons
        if (mapElement.type == MapElementType::Ship) {
            mapElement.icon->setRotation(radToDeg(body.rotation));
        }
    }
    return false;
}

bool MapState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::ToggleMap and start) {
        _stack.popStatesUntil(*this);
        return true;
    }
    return false;
}

bool MapState::handleContinuousAction(const Action& action, sf::Time dt) {
    bool consumeEvent{true};
    switch (action) {
        case Action::ZoomIn:
            _scale *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case Action::ZoomOut:
            _scale /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        default:
            consumeEvent = false;
            break;
    }
    _scale = std::clamp(_scale, _minScale, _maxScale);
    return consumeEvent;
}
