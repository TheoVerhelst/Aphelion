#include <Application.hpp>
#include <components.hpp>
#include <SceneLoader.hpp>

Application::Application(const std::string& setupFile):
    _physicsSystem{_scene},
    _renderSystem{_scene, _shaderManager},
    _gameplaySystem{_scene},
    _lightSystem{_scene},
    _debugOverlay{_gui, _physicsSystem, _scene.view<Body, DebugInfo>(), _textureManager} {
    setFullscreen();
    _gui.setWindow(_window);
    loadResources();
    _gui.loadWidgetsFromFile(_guiFile);
    _sceneCanvas = _gui.get<tgui::CanvasSFML>("sceneCanvas");
    _gui.add(_sceneCanvas);
    _sceneCanvas->moveToBack();
    _lightSystem.setRenderTarget(_sceneCanvas->getRenderTexture());
    _lightSystem.setShader(_shaderManager.get("light"));
    _debugOverlay.buildGui();
    _gameplaySystem.setRenderTarget(_sceneCanvas->getRenderTexture());
    loadScene(_scene, setupFile, _fontManager, _textureManager);
    _currentWindowSize = _window.getSize();
    updateView();
}

void Application::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        // Handle events
        sf::Event event;
        while (_window.pollEvent(event)) {
            // TGUI needs to handle the events first in order for resizing
            // the window to work properly
            _gui.handleEvent(event);

            if (event.type == sf::Event::Closed) {
                _window.close();
            } else if (event.type == sf::Event::Resized) {
                updateView();
            }

            // Dispatch remaining events to various in-game UI
            if (_debugOverlay.handleEvent(event)) {
                continue;
            } else if (_inputManager.handleEvent(event)) {
                continue;
            }
        }

        // Update various systems
        sf::Time elapsedTime{clock.restart()};
        _physicsSystem.updateTime(elapsedTime);
        _renderSystem.update(elapsedTime);
        _debugOverlay.update();
        _gameplaySystem.handleTriggerActions(_inputManager.getTriggerActions());
        _gameplaySystem.handleContinuousActions(elapsedTime, _inputManager.getContinuousActions());
        _lightSystem.update();
        _musicManager.update();

        // Draw graphics
        _sceneCanvas->clear(sf::Color::Transparent);
        _sceneCanvas->draw(_renderSystem, &_shaderManager.get("light"));
        _sceneCanvas->draw(_debugOverlay);
        _sceneCanvas->display();
        _window.clear(sf::Color::Black);
        _window.draw(_backgroundSprite);
        _gui.draw();
        _window.display();
    }
}

void Application::loadResources() {
    _fontManager.loadFromFile("resources/fonts/FreeSans.ttf", "debugFont");
    _textureManager.loadFromFile("resources/gui/play.png", "playButton");
    _textureManager.loadFromFile("resources/gui/play_hover.png", "playHoverButton");
    _textureManager.loadFromFile("resources/gui/pause.png", "pauseButton");
    _textureManager.loadFromFile("resources/gui/pause_hover.png", "pauseHoverButton");
    _textureManager.loadFromFile("resources/textures/background.png", "background");
    _textureManager.loadFromFile("resources/textures/ship.png", "ship");
    _textureManager.loadFromFile("resources/textures/sun.png", "sun");
    _textureManager.loadFromFile("resources/textures/mercury.png", "mercury");
    _textureManager.loadFromFile("resources/textures/venus.png", "venus");
    _textureManager.loadFromFile("resources/textures/earth.png", "earth");
    _textureManager.loadFromFile("resources/textures/mars.png", "mars");
    _textureManager.loadFromFile("resources/textures/rcs.png", "rcs");
    _shaderManager.loadFromFile("resources/shaders/light.frag", "light", sf::Shader::Fragment);
    _musicManager.openFromFile("resources/musics/Aphelion.ogg");

    _backgroundSprite.setTexture(_textureManager.get("background"));
    // Center on an interesting region on the background
    _backgroundSprite.setOrigin(1500, 900);
}

void Application::setFullscreen() {
    const std::vector<sf::VideoMode>& modes{sf::VideoMode::getFullscreenModes()};
    if (modes.size() > 0) {
        // Mode 0 is always the highest resolution
        _window.create(modes[0], "Perihelion");
    }
}

void Application::updateView() {
    sf::View windowView{_window.getView()};
    Vector2f newWindowSize{static_cast<Vector2f>(_window.getSize())};
    windowView.setSize(newWindowSize);
    windowView.setCenter(newWindowSize / 2.f);
    _window.setView(windowView);

    sf::View canvasView{_sceneCanvas->getView()};
    float previousZoom{_currentWindowSize.x / canvasView.getSize().x};
    canvasView.setSize(newWindowSize / previousZoom);
    _sceneCanvas->setView(canvasView);
    _currentWindowSize = _window.getSize();
}
