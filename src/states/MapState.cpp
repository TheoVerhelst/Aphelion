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
        {sf::Keyboard::LShift, MapInput::ZoomIn},
        {sf::Keyboard::RShift, MapInput::ZoomIn},
        {sf::Keyboard::LControl, MapInput::ZoomOut},
        {sf::Keyboard::RControl, MapInput::ZoomOut},
        {sf::Keyboard::M, MapInput::Exit},
        {sf::Keyboard::Escape, MapInput::Exit}
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
    handleContinuousInputs(dt);
    return false;
}

bool MapState::handleEvent(const sf::Event& event) {
    std::vector<std::pair<MapInput, bool>> triggerInputs{_inputManager.getTriggerInputs(event)};
    for (auto& [input, start] : triggerInputs) {
        if (start) {
            switch (input) {
            case MapInput::Exit:
                _stack.popStatesUntil(*this);
                return true;
            default:
                break;
            }
        }
    }
    return false;
}

void MapState::handleContinuousInputs(sf::Time dt) {
    for (MapInput& input : _inputManager.getContinuousInputs()) {
        switch (input) {
        case MapInput::ZoomIn:
            _scale *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case MapInput::ZoomOut:
            _scale /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        default:
            break;
        }
    }
    _scale = std::clamp(_scale, _minScale, _maxScale);
}
