#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/Texture.hpp>
#include <states/GameState.hpp>
#include <states/PauseState.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <ResourceManager.hpp>
#include <Action.hpp>
#include <components/Body.hpp>
#include <components/Animations.hpp>
#include <components/components.hpp>

GameState::GameState(StateStack& stack,
        ResourceManager<sf::Texture>& textureManager,
        ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::Shader>& shaderManager,
        ResourceManager<sf::SoundBuffer>& soundBufferManager,
        SoundSettings& soundSettings,
        const std::filesystem::path& savePath):
    AbstractState{stack},
    _canvas{tgui::CanvasSFML::create()},
    _shaderManager{shaderManager},
    _background{tgui::Picture::create(tguiTextureManager.get("background"))},
    _inputManager{{
        {sf::Keyboard::Z, GameAction::RcsUp},
        {sf::Keyboard::Q, GameAction::RcsLeft},
        {sf::Keyboard::S, GameAction::RcsDown},
        {sf::Keyboard::D, GameAction::RcsRight},
        {sf::Keyboard::A, GameAction::RcsCounterClockwise},
        {sf::Keyboard::E, GameAction::RcsClockwise},
        {sf::Keyboard::Space, GameAction::Engine},
        {sf::Keyboard::LShift, GameAction::ZoomIn},
        {sf::Keyboard::RShift, GameAction::ZoomIn},
        {sf::Keyboard::LControl, GameAction::ZoomOut},
        {sf::Keyboard::RControl, GameAction::ZoomOut},
        {sf::Keyboard::M, GameAction::ToggleMap},
        {sf::Keyboard::LAlt, GameAction::RotateView},
        {sf::Keyboard::Escape, GameAction::Pause}
    }},
    _animationSystem{_scene, soundSettings},
    _collisionSystem{_scene},
    _gameplaySystem{_scene},
    _lightSystem{_scene, _canvas->getRenderTexture(), shaderManager.get("light")},
    _physicsSystem{_scene},
    _renderSystem{_scene},
    _serializer{_scene, textureManager, tguiTextureManager, soundBufferManager} {
    registerComponents();
    // TODO Display a message when the save is invalid (e.g. JSON error), rather than crashing
    _serializer.load(savePath);
}

tgui::Widget::Ptr GameState::buildGui() {
    tgui::Group::Ptr group{tgui::Group::create()};
    group->add(_background);
    group->add(_canvas);
    return group;
}

bool GameState::update(sf::Time dt) {
    handleContinuousActions(dt);
    // Update systems
    _collisionSystem.update();
    _lightSystem.update();
    _animationSystem.update(dt);
    _physicsSystem.update(dt);
    _renderSystem.update();
    // Update the view
    updateView(1.f, false, dt);
    // Draw on the canvas
    _canvas->clear(sf::Color::Transparent);
    _canvas->draw(_renderSystem, &_shaderManager.get("light"));
    _canvas->display();
    return false;
}

bool GameState::handleEvent(const sf::Event& event) {
    std::vector<std::pair<GameAction, bool>> triggerActions{_inputManager.getTriggerActions(event)};
    for (auto& [action, start] : triggerActions) {
        if (start) {
            switch (action) {
            case GameAction::ToggleMap:
                _stack.pushState<MapState>(_scene);
                return true;
            case GameAction::Pause:
                _stack.pushState<PauseState, const SceneSerializer&>(_serializer);
                return true;
            default:
                break;
            }
        }
        if (_animationSystem.handleTriggerAction(action, start)) {
            return true;
        }
    }
    return false;
}

void GameState::handleContinuousActions(sf::Time dt) {
    for (GameAction& action : _inputManager.getContinuousActions()) {
        float zoom{1.f};
        bool rotate{false};
        bool eventConsumed{true};
        switch (action) {
        case GameAction::ZoomIn:
            zoom *= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case GameAction::ZoomOut:
            zoom /= std::pow(_zoomSpeed, dt.asSeconds());
            break;
        case GameAction::RotateView:
            rotate = true;
            break;
        default:
            eventConsumed = false;
            break;
        }
        updateView(zoom, rotate, dt);
        if (not eventConsumed) {
            _gameplaySystem.handleContinuousAction(action, dt);
        }
    }
}

void GameState::registerComponents() {
    _scene.registerComponent<Body>();
    _scene.registerComponent<CircleBody>();
    _scene.registerComponent<PolygonBody>();
    _scene.registerComponent<Animations>();
    _scene.registerComponent<LightSource>();
    _scene.registerComponent<Player>();
    _scene.registerComponent<MapElement>();
    _scene.registerComponent<Sprite>();
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
