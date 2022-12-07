#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Texture.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <Scene.hpp>
#include <Settings.hpp>
#include <ResourceManager.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

MapState::MapState(
        StateStack& stack,
        ResourceManager<tgui::Texture>& tguiTextureManager,
        const Settings& settings,
        Scene& scene):
    AbstractState{stack},
    _tguiTextureManager{tguiTextureManager},
    _scene{scene},
    _inputManager{settings.mapKeyboardMapping, settings.mapControllerMapping},
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
    std::vector<std::pair<MapInput, bool>> inputEvents{_inputManager.getInputEvents(event)};
    bool consumed{false};
    for (auto& [input, start] : inputEvents) {
        if (start and input == MapInput::Exit) {
            _stack.popStatesUntil(*this);
            consumed = true;
        }
    }
    return consumed;
}

bool MapState::handleContinuousInputs(sf::Time dt) {
    if (_inputManager.isActivated(MapInput::ZoomIn)) {
        _scale *= std::pow(_zoomSpeed, dt.asSeconds());
    }
    if (_inputManager.isActivated(MapInput::ZoomOut)) {
        _scale /= std::pow(_zoomSpeed, dt.asSeconds());
    }
    _scale = std::clamp(_scale, _minScale, _maxScale);
    return false;
}
