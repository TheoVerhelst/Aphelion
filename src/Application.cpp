#include <states/GameState.hpp>
#include <states/LoadGameState.hpp>
#include <states/MainMenuState.hpp>
#include <states/MapState.hpp>
#include <states/PauseState.hpp>
#include <states/SaveGameState.hpp>
#include <states/SettingsState.hpp>
#include <Application.hpp>
#include <Paths.hpp>
#include <SceneSerializer.hpp>

Application::Application():
    _window{sf::VideoMode::getDesktopMode(), "Aphelion", sf::Style::Fullscreen},
    _gui{_window},
    _stack{_gui},
    _settings{
        {100, 50, 70},
        sf::VideoMode::getDesktopMode(),
        {
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
        },
        {
            {ControllerButton::X, GameInput::Engine}
        },
        {
            {sf::Keyboard::LShift, MapInput::ZoomIn},
            {sf::Keyboard::RShift, MapInput::ZoomIn},
            {sf::Keyboard::LControl, MapInput::ZoomOut},
            {sf::Keyboard::RControl, MapInput::ZoomOut},
            {sf::Keyboard::M, MapInput::Exit},
            {sf::Keyboard::Escape, MapInput::Exit}
        },
        {
            {ControllerButton::X, MapInput::ZoomIn}
        }
    },
    _musicManager{_settings.soundSettings} {
    _window.setKeyRepeatEnabled(false);
    registerResources();
    tgui::Theme::setDefault("resources/gui/pixelTheme.txt");
    tgui::Font font{"resources/fonts/Ipixelu.ttf"};
    font.setSmooth(false);
    tgui::Font::setGlobalFont(font);
    registerStateBuilders();
    _stack.pushState<MainMenuState>();
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
            _stack.handleEvent(event);
            if (event.type == sf::Event::Closed) {
                _window.close();
            }
        }

        // Update the stack and the music manager
        sf::Time dt{clock.restart()};
        _stack.handleContinuousInputs(dt);
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

void Application::registerResources() {
    tgui::Texture::setDefaultSmooth(false);
    for (auto& path : Paths::getTguiTexturePaths()) {
        _tguiTextureManager.registerFromFile(path, path.stem());
    }
    for (auto& path : Paths::getTexturePaths()) {
        _textureManager.registerFromFile(path, path.stem());
    }
    for (auto& path : Paths::getShaderPaths()) {
        _shaderManager.registerFromFile(path, path.stem(), sf::Shader::Fragment);
    }
    for (auto& path : Paths::getSoundPaths()) {
        _soundBufferManager.registerFromFile(path, path.stem());
    }
    for (auto& path : Paths::getMusicPaths()) {
        _musicManager.openFromFile(path);
    }
}

void Application::registerStateBuilders() {
    _stack.registerStateBuilder<GameState, const std::filesystem::path&>(_stack,
            _textureManager, _tguiTextureManager, _shaderManager,
            _soundBufferManager, _settings);
    _stack.registerStateBuilder<LoadGameState>(_stack);
    _stack.registerStateBuilder<MainMenuState>(_stack, _tguiTextureManager);
    _stack.registerStateBuilder<MapState, Scene&>(_stack, _tguiTextureManager, _settings);
    _stack.registerStateBuilder<PauseState, const SceneSerializer&>(_stack);
    _stack.registerStateBuilder<SaveGameState, const SceneSerializer&>(_stack);
    _stack.registerStateBuilder<SettingsState>(_stack, _settings, _window);
}
