#include <Application.hpp>
#include <components.hpp>
#include <SceneLoader.hpp>
#include <format.hpp>

Application::Application(const std::string& setupFile):
    _window{sf::VideoMode(1200, 600), "Perihelion"},
    _gui{_window},
    _gravitySystem{_scene},
    _renderSystem{_scene} {
    tgui::WidgetFactory::setConstructFunction("SimulationCanvas", std::make_shared<SimulationCanvas>);
    loadResources();
    buildGui();
    loadScene(_scene, setupFile, _fontManager);
}

void Application::run() {
    sf::Clock clock;
    while (_window.isOpen()) {
        // Update physics
        sf::Time elapsedTime{clock.restart()};
        _gravitySystem.updateTime(elapsedTime);
        _simulationCanvas->update(elapsedTime);

        // Handle events
        sf::Event event;
        while (_window.pollEvent(event)) {
            _gui.handleEvent(event);
            if (event.type == sf::Event::Closed) {
                _window.close();
            }

            // Dispatch the event in order, for now only the canvas
            if (_simulationCanvas->handleEvent(event)) {
                continue;
            }
        }

        // Update graphics
        _renderSystem.update();
        updateDisplays();

        // Draw graphics
        _window.clear(sf::Color::Black);
        _simulationCanvas->clear(sf::Color::Black);
        _simulationCanvas->draw(_renderSystem);
        _simulationCanvas->display();
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
}

void Application::buildGui() {
	_gui.loadWidgetsFromFile(_guiFile);
    _simulationCanvas = _gui.get<SimulationCanvas>("simulationCanvas");
    _simulationCanvas->setScene(_scene.view<Body, DebugInfo>());

    // Spin controls. Do not work with GUI text file import for some reason.
    auto timeSpeedControl = tgui::SpinControl::create(-100, 100, 1, 2, 0.1);
    timeSpeedControl->setPosition("timeSpeedLabel.right", "timeSpeedLabel.top");
    timeSpeedControl->setSize(124, 16);
    timeSpeedControl->onValueChange([this] (float value) {
        if(not _paused) {
            _gravitySystem.setTimeScale(value);
        }
    });
    _gui.get<tgui::ChildWindow>("controlsPanel")->add(timeSpeedControl, "timeSpeedControl");

    // Other bindings
    _gui.get<tgui::Button>("stepBackButton")->onPress([this] () {
        pauseTime();
        _gravitySystem.updateSteps(-1);
    });
    _gui.get<tgui::Button>("stepForwardButton")->onPress([this] () {
        pauseTime();
        _gravitySystem.updateSteps(1);
    });
    auto pauseButton = _gui.get<tgui::Button>("pauseButton");
    _gui.get<tgui::Button>("pauseButton")->onPress([this] () {
        if (_paused) {
            resumeTime();
        } else {
            pauseTime();
        }
    });
}

void Application::updateDisplays() {
    _gui.get<tgui::Label>("timeSecondsDisplay")->setText(formatTime(_gravitySystem.getElapsedTime()));
    _gui.get<tgui::Label>("timeStepsDisplay")->setText(std::to_string(_gravitySystem.getStepCounter()));
}

void Application::pauseTime() {
    auto renderer = _gui.get<tgui::Button>("pauseButton")->getRenderer();
    renderer->setTexture(_textureManager.getRef("playButton"));
    renderer->setTextureHover(_textureManager.getRef("playHoverButton"));
    _gravitySystem.setTimeScale(0);
    _paused = true;
}

void Application::resumeTime() {
    auto renderer = _gui.get<tgui::Button>("pauseButton")->getRenderer();
    renderer->setTexture(_textureManager.getRef("pauseButton"));
    renderer->setTextureHover(_textureManager.getRef("pauseHoverButton"));
    _paused = false;
    _gravitySystem.setTimeScale(_gui.get<tgui::SpinControl>("timeSpeedControl")->getValue());
}
