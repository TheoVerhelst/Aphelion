#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/Texture.hpp>
#include <states/GameState.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <ResourceManager.hpp>
#include <SceneLoader.hpp>
#include <Action.hpp>
#include <components.hpp>
#include <DebugInfo.hpp>

GameState::GameState(StateStack& stack,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::Shader>& shaderManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager):
    AbstractState{stack},
    _canvas{tgui::CanvasSFML::create()},
    _shaderManager{shaderManager},
    _background{tgui::Picture::create(tguiTextureManager.get("background"))},
    _collisionSystem{_scene},
    _gameplaySystem{_scene},
    _lightSystem{_scene, _canvas->getRenderTexture(), shaderManager.get("light")},
    _physicsSystem{_scene},
    _renderSystem{_scene} {
    registerComponents();
    loadScene(_scene, _saveFile, fontManager, textureManager, tguiTextureManager, soundBufferManager);
}


tgui::Widget::Ptr GameState::buildGui() {
    tgui::Group::Ptr group{tgui::Group::create()};
    group->add(_background);
    group->add(_canvas);
    return group;
}

void GameState::update(sf::Time dt) {
    // Update systems
    _collisionSystem.update();
    _lightSystem.update();
    _physicsSystem.update(dt);
    _renderSystem.update(dt);
    // Update the view
    updateView(1.f, false, dt);
    // Draw on the canvas
    _canvas->clear(sf::Color::Transparent);
    _canvas->draw(_renderSystem, &_shaderManager.get("light"));
    _canvas->display();
}

bool GameState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    switch (action) {
        case Action::ToggleMap:
            if (start) {
                _stack.pushState(new MapState(_stack, _scene));
                return true;
            }
            break;
        default:
            break;
    }
    return _gameplaySystem.handleTriggerAction(actionPair);
}

bool GameState::handleContinuousAction(const Action& action, sf::Time dt) {
    float zoom{1.f};
    bool rotate{false};
    bool consumeEvent{true};
    switch (action) {
        case Action::ZoomIn:
            zoom *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case Action::ZoomOut:
            zoom /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case Action::RotateView:
            rotate = true;
            break;
        default:
            consumeEvent = false;
            break;
    }
    updateView(zoom, rotate, dt);
    return consumeEvent or _gameplaySystem.handleContinuousAction(action, dt);
}

void GameState::registerComponents() {
    _scene.registerComponent<Body>();
    _scene.registerComponent<CircleBody>();
    _scene.registerComponent<ConvexBody>();
    _scene.registerComponent<Collider>();
    _scene.registerComponent<AnimationComponent>();
    _scene.registerComponent<LightSource>();
    _scene.registerComponent<Shadow>();
    _scene.registerComponent<Player>();
    _scene.registerComponent<DebugInfo>();
    _scene.registerComponent<MapElement>();
    _scene.registerComponent<sf::Sprite>();
}

void GameState::updateView(float zoom, bool rotate, sf::Time dt) {
    const Body& playerBody{_scene.getComponent<Body>(_scene.findUnique<Player>())};
    sf::View view{_canvas->getRenderTexture().getView()};
    const Vector2f viewSize{view.getSize() * zoom};
    view.setSize(clampVector(viewSize, _minViewSize, _maxViewSize));
    view.setCenter(playerBody.position);
    if (rotate) {
        float target{playerBody.rotation};
        float current{degToRad(view.getRotation())};
        if (target - current > pi) {
            current += 2.f * pi;
        } else if (current - target > pi) {
            target += 2.f * pi;
        }
        float newAngle{current + _rotationSpeed * dt.asSeconds() * (target > current ? 1 : -1)};
        if (target > current) {
            view.setRotation(radToDeg(std::min(newAngle, target)));
        } else {
            view.setRotation(radToDeg(std::max(newAngle, target)));
        }
    }
    _canvas->getRenderTexture().setView(view);
}