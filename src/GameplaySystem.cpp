#include <GameplaySystem.hpp>
#include <vector.hpp>
#include <iostream>

GameplaySystem::GameplaySystem(const SceneView<Body, Player>& scene):
    _scene{scene} {
}

void GameplaySystem::handleActions(const sf::Time& dt, const std::set<Action>& actions) {
    for(EntityId id : _scene) {
        Vector2d dv{0, 0};
        double dw{0};
        const double engineAccel{500};
        const double rcsLinearAccel{50};
        const double rcsCircularAccel{50};

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
            }
        }

        Body& body{_scene.getComponent<Body>(id)};
        body.velocity += rotate(dv, body.rotation) * static_cast<double>(dt.asSeconds());
        body.angularVelocity += dw * dt.asSeconds();

        if (_renderTarget != nullptr) {
            sf::View playerView{_renderTarget->getView()};
            playerView.setCenter(static_cast<Vector2f>(body.position));
            playerView.setRotation(body.rotation * 180. / pi);
            _renderTarget->setView(playerView);
        }
        break;
    }
}

void GameplaySystem::setRenderTarget(sf::RenderTarget& renderTarget) {
    _renderTarget = &renderTarget;
}
