#include <stdexcept>
#include <cmath>
#include <cassert>
#include <systems/GameplaySystem.hpp>
#include <Animation.hpp>
#include <components.hpp>

GameplaySystem::GameplaySystem(Scene& scene, sf::RenderTarget& renderTarget):
    _scene{scene},
    _renderTarget{renderTarget} {
}

void GameplaySystem::update(const TriggerAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, start] = actionPair;

    // Play the player animations
    auto& animations{_scene.getComponent<AnimationComponent>(playerId).animations};
    auto animationIt = animations.find(action);
    if (animationIt != animations.end()) {
        if (start) {
            animationIt->second.start();
        } else {
            animationIt->second.stop();
        }
    }
}

void GameplaySystem::update(const ContinuousAction& actionPair) {
    const EntityId playerId{_scene.findUnique<Player>()};
    auto& [action, dt] = actionPair;

    Vector2f dv{0, 0};
    float dw{0};
    float zoom{1.f};
    bool rotateView{false};
    const float engineAccel{200};
    const float rcsLinearAccel{50};
    const float rcsCircularAccel{10};
    const float zoomSpeed{15};

    switch (action) {
        case Action::Engine:
            dv += {0, -engineAccel};
            break;
        case Action::RcsUp:
            dv += {0, -rcsLinearAccel};
            break;
        case Action::RcsDown:
            dv += {0, rcsLinearAccel};
            break;
        case Action::RcsLeft:
            dv += {-rcsLinearAccel, 0};
            break;
        case Action::RcsRight:
            dv += {rcsLinearAccel, 0};
            break;
        case Action::RcsClockwise:
            dw += rcsCircularAccel;
            break;
        case Action::RcsCounterClockwise:
            dw -= rcsCircularAccel;
            break;
        case Action::ZoomIn:
            zoom *= std::pow(zoomSpeed, dt.asSeconds());
            break;
        case Action::ZoomOut:
            zoom /= std::pow(zoomSpeed, dt.asSeconds());
            break;
        case Action::RotateView:
            rotateView = true;
            break;
        case Action::ToggleMap:
            break;
        default:
            assert(false);
    }

    Body& body{_scene.getComponent<Body>(playerId)};
    body.velocity += rotate(dv, body.rotation) * dt.asSeconds();
    body.angularVelocity += dw * dt.asSeconds();

    // Update the view
    sf::View playerView{_renderTarget.getView()};
    Vector2f viewSize{playerView.getSize() * zoom};
    const float aspectRatio{viewSize.x / viewSize.y};
    if (viewSize.x > maxViewSize.x) {
        viewSize = {maxViewSize.x, maxViewSize.x / aspectRatio};
    }
    if (viewSize.y > maxViewSize.y) {
        viewSize = {maxViewSize.y * aspectRatio, maxViewSize.y};
    }
    if (viewSize.x < minViewSize.x) {
        viewSize = {minViewSize.x, minViewSize.x / aspectRatio};
    }
    if (viewSize.y < minViewSize.y) {
        viewSize = {minViewSize.y * aspectRatio, minViewSize.y};
    }

    playerView.setSize(viewSize);
    if (rotateView) {
        playerView.setRotation(body.rotation * 180 / pi);
    }
    _renderTarget.setView(playerView);
}

void GameplaySystem::update(const sf::Time&) {
    // Update the view
    sf::View playerView{_renderTarget.getView()};
    Body& body{_scene.getComponent<Body>(_scene.findUnique<Player>())};
    playerView.setCenter(body.position);
    _renderTarget.setView(playerView);
}
