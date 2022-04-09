#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/Texture.hpp>
#include <states/GameState.hpp>
#include <states/PauseState.hpp>
#include <states/StateStack.hpp>
#include <states/MapState.hpp>
#include <ResourceManager.hpp>
#include <Input.hpp>
#include <GameEvent.hpp>
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
        {sf::Keyboard::Z, GameInput::RcsUp},
        {sf::Keyboard::Q, GameInput::RcsLeft},
        {sf::Keyboard::S, GameInput::RcsDown},
        {sf::Keyboard::D, GameInput::RcsRight},
        {sf::Keyboard::A, GameInput::RcsCounterClockwise},
        {sf::Keyboard::E, GameInput::RcsClockwise},
        {sf::Keyboard::Space, GameInput::Engine},
        {sf::Keyboard::LShift, GameInput::ZoomIn},
        {sf::Keyboard::RShift, GameInput::ZoomIn},
        {sf::Keyboard::LControl, GameInput::ZoomOut},
        {sf::Keyboard::RControl, GameInput::ZoomOut},
        {sf::Keyboard::M, GameInput::ToggleMap},
        {sf::Keyboard::LAlt, GameInput::RotateView},
        {sf::Keyboard::Escape, GameInput::Pause}
    }},
    _eventMapping{{
        {GameInput::Engine, GameEventType::Engine},
        {GameInput::RcsUp, GameEventType::RcsUp},
        {GameInput::RcsDown, GameEventType::RcsDown},
        {GameInput::RcsLeft, GameEventType::RcsLeft},
        {GameInput::RcsRight, GameEventType::RcsRight},
        {GameInput::RcsClockwise, GameEventType::RcsClockwise},
        {GameInput::RcsCounterClockwise, GameEventType::RcsCounterClockwise}
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
    handleContinuousInputs(dt);
    // Update systems
    _collisionSystem.update();
    _lightSystem.update();
    _animationSystem.update(dt);
    _physicsSystem.update(dt);
    _renderSystem.update();

    while (not _eventQueue.empty()) {
        const GameEvent& event{_eventQueue.front()};
        _animationSystem.handleGameEvent(event);
        _gameplaySystem.handleGameEvent(event, dt);
        _eventQueue.pop();
    }

    // Update the view
    updateView(1.f, false, dt);
    // Draw on the canvas
    _canvas->clear(sf::Color::Transparent);
    _canvas->draw(_renderSystem, &_shaderManager.get("light"));
    _canvas->display();
    return false;
}

bool GameState::handleEvent(const sf::Event& event) {
    const EntityId playerId{_scene.findUnique<Player>()};
    bool consumed{false};
    std::vector<std::pair<GameInput, bool>> triggerInputs{_inputManager.getTriggerInputs(event)};
    for (auto& [input, start] : triggerInputs) {
        auto it = _eventMapping.find(input);
        if (it != _eventMapping.end()) {
            GameEvent gameEvent{
                it->second,
                start ? EventStatus::Start : EventStatus::Stop,
                playerId,
                0
            };
            _eventQueue.push(gameEvent);
            consumed = true;
        } else if (start) {
            switch (input) {
            case GameInput::ToggleMap:
                _stack.pushState<MapState>(_scene);
                consumed = true;
                break;
            case GameInput::Pause:
                _stack.pushState<PauseState, const SceneSerializer&>(_serializer);
                consumed = true;
                break;
            default:
                break;
            }
        }
    }
    return consumed;
}

void GameState::handleContinuousInputs(sf::Time dt) {
    const EntityId playerId{_scene.findUnique<Player>()};

    for (GameInput& input : _inputManager.getContinuousInputs()) {
        auto it = _eventMapping.find(input);
        if (it != _eventMapping.end()) {
            _eventQueue.emplace(it->second, EventStatus::Ongoing, playerId, 0);
        } else {
            float zoom{1.f};
            bool rotate{false};
            switch (input) {
            case GameInput::ZoomIn:
                zoom *= std::pow(_zoomSpeed, dt.asSeconds());
                break;
            case GameInput::ZoomOut:
                zoom /= std::pow(_zoomSpeed, dt.asSeconds());
                break;
            case GameInput::RotateView:
                rotate = true;
                break;
            default:
                break;
            }
            updateView(zoom, rotate, dt);
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
