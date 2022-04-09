#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Texture.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <Scene.hpp>
#include <ResourceManager.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

MapState::MapState(StateStack& stack, ResourceManager<tgui::Texture>& tguiTextureManager, Scene& scene):
    AbstractState{stack},
    _tguiTextureManager{tguiTextureManager},
    _scene{scene},
    _inputManager{{
        {sf::Keyboard::LShift, MapAction::ZoomIn},
        {sf::Keyboard::RShift, MapAction::ZoomIn},
        {sf::Keyboard::LControl, MapAction::ZoomOut},
        {sf::Keyboard::RControl, MapAction::ZoomOut},
        {sf::Keyboard::M, MapAction::Exit},
        {sf::Keyboard::Escape, MapAction::Exit}
    }},
    _background{tgui::Picture::create(_tguiTextureManager.get("mapBackground"))} {
}

tgui::Widget::Ptr MapState::buildGui() {
    for (auto& [id, mapElement] : _scene.view<MapElement>()) {
        _mapIcons->add(mapElement.icon);
    }
    tgui::Group::Ptr group{tgui::Group::create()};
    group->add(_background);
    group->add(_mapIcons);
    return group;
}

bool MapState::update(sf::Time dt) {
    const Body& playerBody{_scene.getComponent<Body>(_scene.findUnique<Player>())};
    const Vector2f playerPos{playerBody.position};
    const Vector2f mapSize{_mapIcons->getSize()};

    for (auto& [id, body, mapElement] : _scene.view<Body, MapElement>()) {
        // Compute the position of the map element on the screen.
        Vector2f screenPos{(body.position - playerPos) / _scale};
        screenPos += mapSize / 2.f;
        mapElement.icon->setPosition(static_cast<tgui::Vector2f>(screenPos));
        mapElement.icon->setRotation(radToDeg(body.rotation));
    }
    handleContinuousActions(dt);
    return false;
}

bool MapState::handleEvent(const sf::Event& event) {
    std::vector<std::pair<MapAction, bool>> triggerActions{_inputManager.getTriggerActions(event)};
    for (auto& [action, start] : triggerActions) {
        if (start) {
            switch (action) {
            case MapAction::Exit:
                _stack.popStatesUntil(*this);
                return true;
            default:
                break;
            }
        }
    }
    return false;
}

void MapState::handleContinuousActions(sf::Time dt) {
    for (MapAction& action : _inputManager.getContinuousActions()) {
        switch (action) {
        case MapAction::ZoomIn:
            _scale *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case MapAction::ZoomOut:
            _scale /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        default:
            break;
        }
    }
    _scale = std::clamp(_scale, _minScale, _maxScale);
}
