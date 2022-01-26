#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <TGUI/TGUI.hpp>
#include <systems/MapSystem.hpp>
#include <Action.hpp>
#include <Scene.hpp>
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

        // Set the view size
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


void MapSystem::update(const ContinuousAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, dt] = actionPair;

    float zoom{1.f};
    bool rotateView{false};
    switch (action) {
        case Action::ZoomIn:
            zoom *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case Action::ZoomOut:
            zoom /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case Action::RotateView:
            rotateView = true;
            break;
        default:
            break;
    }

    // Update the view
    sf::View playerView{_renderTarget.getView()};
    Vector2f viewSize{playerView.getSize() * zoom};
    if (_mapView) {
        viewSize = clampVector(viewSize, _minMapViewSize, _maxMapViewSize);
    } else {
        viewSize = clampVector(viewSize, _minNormalViewSize, _maxNormalViewSize);
    }
    playerView.setSize(viewSize);
    if (rotateView) {
        Body& body{_scene.getComponent<Body>(playerId)};
        float target{body.rotation};
        float current{degToRad(playerView.getRotation())};
        if (target - current > pi) {
            current += 2.f * pi;
        } else if (current - target > pi) {
            target += 2.f * pi;
        }
        float newAngle{current + _rotationSpeed * dt.asSeconds() * (target > current ? 1 : -1)};
        if (target > current) {
            playerView.setRotation(radToDeg(std::min(newAngle, target)));
        } else {
            playerView.setRotation(radToDeg(std::max(newAngle, target)));
        }
    }
    _renderTarget.setView(playerView);
}


void MapSystem::update(const sf::Time&) {
    // Update map elements
    for (EntityId id : _scene.view<Body, MapElement>()) {
        const Body& body{_scene.getComponent<Body>(id)};
        MapElement& mapElement{_scene.getComponent<MapElement>(id)};

        // Compute the position of the map element on the screen. Note that we
        // don't rotate the map icon.
        Vector2i screenPosition{_renderTarget.mapCoordsToPixel(body.position)};
        mapElement.icon->setPosition(tgui::Vector2f(static_cast<Vector2f>(screenPosition)));
        // Except for ship icons
        if (mapElement.type == MapElementType::Ship) {
            float rotation{radToDeg(body.rotation) - _renderTarget.getView().getRotation()};
            mapElement.icon->setRotation(rotation);
        }
    }

    // Update the view
    sf::View playerView{_renderTarget.getView()};
    Body& body{_scene.getComponent<Body>(_scene.findUnique<Player>())};
    playerView.setCenter(body.position);
    _renderTarget.setView(playerView);
}

Vector2f MapSystem::clampVector(Vector2f v, const Vector2f& min, const Vector2f& max) {
    const float aspectRatio{v.x / v.y};
    if (v.x > max.x) {
        v = {max.x, max.x / aspectRatio};
    }
    if (v.y > max.y) {
        v = {max.y * aspectRatio, min.y};
    }
    if (v.x < min.x) {
        v = {min.x, min.x / aspectRatio};
    }
    if (v.y < min.y) {
        v = {min.y * aspectRatio, min.y};
    }
    return v;
}
