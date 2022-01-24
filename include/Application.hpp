#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Scene.hpp>
#include <systems/CollisionSystem.hpp>
#include <systems/GameplaySystem.hpp>
#include <systems/LightSystem.hpp>
#include <systems/MapSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <DebugOverlay.hpp>
#include <InputManager.hpp>
#include <ResourceManager.hpp>
#include <MusicManager.hpp>
#include <Observer.hpp>

class Application {
public:
    Application(const std::string& setupFile);
    void run();

private:
    // Main objects
    sf::RenderWindow _window;
    tgui::Gui _gui;
    Scene _scene;
    tgui::CanvasSFML::Ptr _sceneCanvas;

    // Systems
    CollisionSystem _collisionSystem;
    GameplaySystem _gameplaySystem;
    LightSystem _lightSystem;
    MapSystem _mapSystem;
    PhysicsSystem _physicsSystem;
    RenderSystem _renderSystem;
    DebugOverlay _debugOverlay;
    InputManager _inputManager;
    EventSource<const sf::Time&> _timeEventSource;

    // Resource managers
    ResourceManager<sf::Font> _fontManager;
    ResourceManager<sf::Texture> _textureManager;
    ResourceManager<tgui::Texture> _tguiTextureManager;
    ResourceManager<sf::Shader> _shaderManager;
    MusicManager _musicManager;

    // Others
    const std::string _guiFile{"gui.txt"};
    sf::Sprite _backgroundSprite;
    Vector2u _currentWindowSize;

    void registerComponents();
    void registerObservers();
    void loadResources();
    void updateView();
};

#endif // APPLICATION_HPP
