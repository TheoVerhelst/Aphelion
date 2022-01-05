#include <Application.hpp>
#include <components.hpp>
#include <SceneLoader.hpp>

Application::Application(const std::string& setupFile):
    _window{sf::VideoMode(1200, 600), "Perihelion"},
    _gui{_window},
    _physicsSystem{_scene},
    _renderSystem{_scene},
    _debugOverlay{_gui, _physicsSystem, _scene.view<Body, DebugInfo>(), _textureManager} {
    loadResources();

    tgui::WidgetFactory::setConstructFunction("CanvasSFML", std::make_shared<tgui::CanvasSFML>);
	_gui.loadWidgetsFromFile(_guiFile);
    _sceneCanvas = _gui.get<tgui::CanvasSFML>("sceneCanvas");
    _sceneCanvas->moveToBack();
    _debugOverlay.buildGui();

    loadScene(_scene, setupFile, _fontManager, _textureManager);
}

void Application::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        // Update physics
        sf::Time elapsedTime{clock.restart()};
        _physicsSystem.updateTime(elapsedTime);

        // Handle events
        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                _window.close();
            }
            // Dispatch the event in order
            if (_gui.handleEvent(event)) {
                continue;
            } else if (_debugOverlay.handleEvent(event)) {
                continue;
            }
        }

        // Update graphics
        _renderSystem.update();
        _debugOverlay.update();

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
}
