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
#include <Event.hpp>
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
    _eventMapping{
        {GameInput::Engine, {0, false, Event::EngineEvent()}},
        {GameInput::RcsUp, {0, false, Event::RcsEvent::Up}},
        {GameInput::RcsDown, {0, false, Event::RcsEvent::Down}},
        {GameInput::RcsLeft, {0, false, Event::RcsEvent::Left}},
        {GameInput::RcsRight, {0, false, Event::RcsEvent::Right}},
        {GameInput::RcsClockwise, {0, false, Event::RcsEvent::Clockwise}},
        {GameInput::RcsCounterClockwise, {0, false, Event::RcsEvent::CounterClockwise}}
    },
    _animationSystem{_scene, soundSettings},
    _autoPilotSystem{_scene},
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
    processtriggerEventsQueue();
    // Update systems
    _collisionSystem.update();
    _lightSystem.update();
    _animationSystem.update(dt);
    _physicsSystem.update(dt);
    _renderSystem.update();
    _gameplaySystem.update(dt);
    // Update the view
    updateView(1.f, false, dt);
    // Draw on the canvas
    _canvas->clear(sf::Color::Transparent);
    _canvas->draw(_renderSystem, &_shaderManager.get("light"));
    _canvas->display();
    return false;
}

bool GameState::handleEvent(const sf::Event& sfEvent) {
    const EntityId playerId{_scene.findUnique<Player>()};
    ShipControl& shipControl{_scene.getComponent<ShipControl>(playerId)};
    bool consumed{false};
    std::vector<std::pair<GameInput, bool>> inputEvents{_inputManager.getInputEvents(sfEvent)};
    for (auto& [input, start] : inputEvents) {
        // If it is a gameplay input event
        auto it = _eventMapping.find(input);
        if (it != _eventMapping.end()) {
            // Set the entity and the event data
            Event event{it->second};
            event.entity = playerId;
            event.start = start;
            // Update the ship control component. We have to do it in this class
            // rather than in, say, AutoPilotSystem because only here we know
            // that the event is due to user input and not due to gameplay
            // logic. So we know that we have to set shipControl.playerControls
            // rather than shipControl.autoControls.
            switch(input) {
            case GameInput::Engine:
                shipControl.playerControls.engine = start;
                break;
            case GameInput::RcsUp:
                shipControl.playerControls.rcsUp = start;
                break;
            case GameInput::RcsDown:
                shipControl.playerControls.rcsDown = start;
                break;
            case GameInput::RcsLeft:
                shipControl.playerControls.rcsLeft = start;
                break;
            case GameInput::RcsRight:
                shipControl.playerControls.rcsRight = start;
                break;
            case GameInput::RcsClockwise:
                shipControl.playerControls.rcsClockwise = start;
                break;
            case GameInput::RcsCounterClockwise:
                shipControl.playerControls.rcsCounterClockwise = start;
                break;
            default:
                break;
            }
            _eventQueue.push(event);
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

bool GameState::handleContinuousInputs(sf::Time dt) {
    float zoom{1.f};
    bool rotate{false};
    if (_inputManager.isActivated(GameInput::ZoomIn)) {
        zoom *= std::pow(_zoomSpeed, dt.asSeconds());
    }
    if (_inputManager.isActivated(GameInput::ZoomOut)) {
        zoom /= std::pow(_zoomSpeed, dt.asSeconds());
    }
    if (_inputManager.isActivated(GameInput::RotateView)) {
        rotate = true;
    }
    updateView(zoom, rotate, dt);
    return true;
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
    _scene.registerComponent<ShipControl>();
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

void GameState::processtriggerEventsQueue() {
    // Add events from systems that generate them
    auto autoPilotQueue = _autoPilotSystem.queueEvents();
    while (not autoPilotQueue.empty()) {
        _eventQueue.push(autoPilotQueue.front());
        autoPilotQueue.pop();
    }
    auto collisionQueue = _collisionSystem.queueEvents();
    while (not collisionQueue.empty()) {
        _eventQueue.push(collisionQueue.front());
        collisionQueue.pop();
    }
    // Process them
    while (not _eventQueue.empty()) {
        const Event& event{_eventQueue.front()};
        _animationSystem.handleEvent(event);
        _eventQueue.pop();
    }
}
