#include <TGUI/TGUI.hpp>
#include <systems/MapSystem.hpp>
#include <components.hpp>

MapSystem::MapSystem(Scene& scene, sf::RenderTarget& renderTarget):
    _scene{scene},
    _renderTarget{renderTarget} {
}

void MapSystem::update(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::ToggleMap and start) {
        // Toggle the map
        _mapView = not _mapView;
        for (EntityId id : _scene.view<MapElement>()) {
            tgui::Picture::Ptr icon{_scene.getComponent<MapElement>(id).icon};
            icon->setVisible(not icon->isVisible());
        }

        // Update the view
        sf::View playerView{_renderTarget.getView()};
        Vector2f viewSize{playerView.getSize()};

        if (_mapView) {
            _normalViewSize = viewSize;
            viewSize = _mapViewSize;
        } else {
            _mapViewSize = viewSize;
            viewSize = _normalViewSize;
        }

        playerView.setSize(viewSize);
        _renderTarget.setView(playerView);
    }
}

void MapSystem::update(const sf::Time&) {
    for (EntityId id : _scene.view<Body, MapElement>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        MapElement& mapElement{_scene.getComponent<MapElement>(id)};

        // Compute the position of the map element on the screen. Note that we
        // don't rotate the map icon.
        Vector2i screenPosition{_renderTarget.mapCoordsToPixel(body.position)};
        mapElement.icon->setPosition(tgui::Vector2f(static_cast<Vector2f>(screenPosition)));
        // Except for ship icons
        if (mapElement.type == MapElementType::Ship) {
            float rotation{(body.rotation * 180.f / pi) - _renderTarget.getView().getRotation()};
            mapElement.icon->setRotation(rotation);
        }
    }
}
