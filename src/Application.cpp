#include <Application.hpp>
#include <components.hpp>
#include <SceneLoader.hpp>

Application::Application(const std::string& setupFile):
    _gui{_window},
    _physicsSystem{_scene},
    _renderSystem{_scene},
    _gameplaySystem{_scene},
    _debugOverlay{_gui, _physicsSystem, _scene.view<Body, DebugInfo>(), _textureManager} {
    loadResources();

    tgui::WidgetFactory::setConstructFunction("CanvasSFML", std::make_shared<tgui::CanvasSFML>);
	_gui.loadWidgetsFromFile(_guiFile);
    _sceneCanvas = _gui.get<tgui::CanvasSFML>("sceneCanvas");
    _sceneCanvas->moveToBack();
    _debugOverlay.buildGui();
    _gameplaySystem.setRenderTarget(_sceneCanvas->getRenderTexture());
    loadScene(_scene, setupFile, _fontManager, _textureManager);
    setFullscreen();
}

void Application::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        // Handle events
        sf::Event event;
        while (_window.pollEvent(event)) {
            // Dispatch the event in order
            _gui.handleEvent(event);

            if (event.type == sf::Event::Closed) {
                _window.close();
            } else if (event.type == sf::Event::Resized) {
                Vector2f newSize{static_cast<float>(event.size.width), static_cast<float>(event.size.height)};
                sf::View canvasView{_sceneCanvas->getView()};
                sf::View windowView{_window.getView()};
                const float zoomFactor{2.f};
                canvasView.setSize(newSize / zoomFactor);
                windowView.setSize(newSize);
                windowView.setCenter(newSize / 2.f);
                _sceneCanvas->setView(canvasView);
                _window.setView(windowView);
            }
            // TGUI needs to handle closed and resized events as well, so no "else if"

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

        // Draw graphics
        _sceneCanvas->clear(sf::Color::Black);
        _sceneCanvas->draw(_renderSystem);
        _sceneCanvas->draw(_debugOverlay);
        _sceneCanvas->display();
        _window.clear(sf::Color::Black);
        _gui.draw();
        _window.display();
    }
}

void Application::loadResources() {
    _fontManager.loadFromFile("resources/FreeSans.ttf", "debugFont");
    _textureManager.loadFromFile("resources/play.png", "playButton");
    _textureManager.loadFromFile("resources/play_hover.png", "playHoverButton");
    _textureManager.loadFromFile("resources/pause.png", "pauseButton");
    _textureManager.loadFromFile("resources/pause_hover.png", "pauseHoverButton");
    _textureManager.loadFromFile("resources/ship.png", "ship");
    _textureManager.loadFromFile("resources/sun.png", "sun");
    _textureManager.loadFromFile("resources/asteroid.png", "asteroid");
    _textureManager.loadFromFile("resources/rcs.png", "rcs");
}

void Application::setFullscreen() {
    const std::vector<sf::VideoMode>& modes{sf::VideoMode::getFullscreenModes()};
    if (modes.size() > 0) {
        // Mode 0 is always the highest resolution
        _window.create(sf::VideoMode::getDesktopMode(), "Perihelion");//, sf::Style::Fullscreen);
    }
}
