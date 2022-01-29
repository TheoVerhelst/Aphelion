#include <states/GameState.hpp>
#include <states/MapState.hpp>
#include <states/PauseState.hpp>
#include <states/SettingsState.hpp>
#include <Application.hpp>

Application::Application():
    _window{sf::VideoMode::getDesktopMode(), "Aphelion",  sf::Style::Fullscreen},
    _gui{_window},
    _stack{_gui},
    _soundSettings{100, 50, 70},
    _musicManager{_soundSettings} {
    _window.setKeyRepeatEnabled(false);
    loadResources();
    tgui::Theme::setDefault("resources/gui/pixelTheme.txt");
    tgui::Font font{"resources/fonts/Ipixelu.ttf"};
    font.setSmooth(false);
    tgui::Font::setGlobalFont(font);
    registerStateBuilders();
    _stack.pushState<GameState>();
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
            std::vector<TriggerAction> triggerActions{_inputManager.getTriggerActions(event)};
            for (auto& action : triggerActions) {
                _stack.handleTriggerAction(action);
            }
            if (event.type == sf::Event::Closed) {
                _window.close();
            }
        }

        // Update various systems
        sf::Time dt{clock.restart()};
        std::vector<Action> continuousActions{_inputManager.getContinuousActions()};
        for (auto& action : continuousActions) {
            _stack.handleContinuousAction(action, dt);
        }
        _stack.update(dt);
        _musicManager.update();

        // Draw graphics
        _window.clear(sf::Color::Black);
        _gui.draw();
        _window.display();

        if (_stack.isEmpty()) {
            _window.close();
        }
    }
}

void Application::loadResources() {
    tgui::Texture::setDefaultSmooth(false);
    _fontManager.loadFromFile("resources/fonts/FreeSans.ttf", "debugFont");
    _tguiTextureManager.loadFromFile("resources/gui/play.png", "playButton");
    _tguiTextureManager.loadFromFile("resources/gui/playHover.png", "playHoverButton");
    _tguiTextureManager.loadFromFile("resources/gui/pause.png", "pauseButton");
    _tguiTextureManager.loadFromFile("resources/gui/pauseHover.png", "pauseHoverButton");
    _tguiTextureManager.loadFromFile("resources/gui/sunIcon.png", "sunIcon");
    _tguiTextureManager.loadFromFile("resources/gui/mercuryIcon.png", "mercuryIcon");
    _tguiTextureManager.loadFromFile("resources/gui/venusIcon.png", "venusIcon");
    _tguiTextureManager.loadFromFile("resources/gui/earthIcon.png", "earthIcon");
    _tguiTextureManager.loadFromFile("resources/gui/marsIcon.png", "marsIcon");
    _tguiTextureManager.loadFromFile("resources/gui/shipIcon.png", "shipIcon");
    _tguiTextureManager.loadFromFile("resources/textures/background.png", "background");
    _tguiTextureManager.loadFromFile("resources/textures/mapBackground.png", "mapBackground");
    _textureManager.loadFromFile("resources/textures/ship.png", "ship");
    _textureManager.loadFromFile("resources/textures/sun.png", "sun");
    _textureManager.loadFromFile("resources/textures/mercury.png", "mercury");
    _textureManager.loadFromFile("resources/textures/venus.png", "venus");
    _textureManager.loadFromFile("resources/textures/earth.png", "earth");
    _textureManager.loadFromFile("resources/textures/mars.png", "mars");
    _textureManager.loadFromFile("resources/textures/rcs.png", "rcs");
    _shaderManager.loadFromFile("resources/shaders/light.frag", "light", sf::Shader::Fragment);
    _soundBufferManager.loadFromFile("resources/sounds/mainEngine.wav", "mainEngine");
    _soundBufferManager.loadFromFile("resources/sounds/rcs.wav", "rcs");
    _soundBufferManager.loadFromFile("resources/sounds/landing.wav", "landing");
    _musicManager.openFromFile("resources/musics/Aphelion.ogg");
}

void Application::registerStateBuilders() {
    _stack.registerStateBuilder<GameState>(StateStack::StateBuilder<>([this] {
        return new GameState(_stack, _fontManager, _textureManager,
            _tguiTextureManager, _shaderManager, _soundBufferManager, _soundSettings);
    }));
    _stack.registerStateBuilder<MapState>(StateStack::StateBuilder<Scene&>([this] (Scene& scene) {
        return new MapState(_stack, scene, _tguiTextureManager);
    }));
    _stack.registerStateBuilder<PauseState>(StateStack::StateBuilder<>([this] {
        return new PauseState(_stack);
    }));
    _stack.registerStateBuilder<SettingsState>(StateStack::StateBuilder<>([this] {
        return new SettingsState(_stack, _soundSettings);
    }));
}
