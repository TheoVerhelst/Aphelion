#include <stdexcept>
#include <cmath>
#include <GameplaySystem.hpp>
#include <Animation.hpp>
#include <components.hpp>

GameplaySystem::GameplaySystem(Scene& scene):
    _scene{scene} {
}

void GameplaySystem::handleTriggerActions(const std::map<Action, bool>& actions) {
    for(EntityId id : _scene.view<AnimationComponent, Player>()) {
        AnimationComponent& animations{_scene.getComponent<AnimationComponent>(id)};
        for (auto& [action, start] : actions) {
            auto animationIt = animations.find(action);
            if (animationIt != animations.end()) {
                if (start) {
                    animationIt->second.start();
                } else {
                    animationIt->second.stop();
                }
            }
        }
        break;
    }
}

void GameplaySystem::handleContinuousActions(const sf::Time& dt, const std::set<Action>& actions) {
    for(EntityId id : _scene.view<Body, Player>()) {
        Vector2d dv{0, 0};
        double dw{0};
        float zoom{1.f};
        const double engineAccel{200};
        const double rcsLinearAccel{50};
        const double rcsCircularAccel{10};
        const float zoomSpeed{15};

        for (auto action : actions) {
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
            }
        }

        Body& body{_scene.getComponent<Body>(id)};
        body.velocity += rotate(dv, body.rotation) * static_cast<double>(dt.asSeconds());
        body.angularVelocity += dw * dt.asSeconds();

        if (_renderTarget != nullptr) {
            sf::View playerView{_renderTarget->getView()};
            playerView.setCenter(static_cast<Vector2f>(body.position));
            playerView.setSize(playerView.getSize() * zoom);
            _renderTarget->setView(playerView);
        }
        break;
    }
}

void GameplaySystem::setRenderTarget(sf::RenderTarget& renderTarget) {
    _renderTarget = &renderTarget;
}
