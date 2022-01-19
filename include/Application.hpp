#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <Scene.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/GameplaySystem.hpp>
#include <systems/LightSystem.hpp>
#include <DebugOverlay.hpp>
#include <InputManager.hpp>
#include <ResourceManager.hpp>
#include <MusicManager.hpp>

class Application {
public:
    Application(const std::string& setupFile);
    void run();

private:
    const std::string _guiFile{"gui.txt"};
    sf::RenderWindow _window;
    tgui::Gui _gui;
    Scene _scene;
    PhysicsSystem _physicsSystem;
    RenderSystem _renderSystem;
    GameplaySystem _gameplaySystem;
    LightSystem _lightSystem;
    DebugOverlay _debugOverlay;
    InputManager _inputManager;
    tgui::CanvasSFML::Ptr _sceneCanvas;
    ResourceManager<sf::Font> _fontManager;
    ResourceManager<sf::Texture> _textureManager;
    ResourceManager<sf::Shader> _shaderManager;
    sf::Sprite _backgroundSprite;
    MusicManager _musicManager;
    Vector2u _currentWindowSize;

    void registerComponents();
    void loadResources();
    void setFullscreen();
    void updateView();
};

#endif // APPLICATION_HPP
