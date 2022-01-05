#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <PhysicsSystem.hpp>
#include <RenderSystem.hpp>
#include <Scene.hpp>
#include <GameplaySystem.hpp>
#include <InputManager.hpp>
#include <ResourceManager.hpp>
#include <DebugOverlay.hpp>

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
    DebugOverlay _debugOverlay;
    InputManager _inputManager;
    tgui::CanvasSFML::Ptr _sceneCanvas;
    ResourceManager<sf::Font> _fontManager;
    ResourceManager<sf::Texture> _textureManager;

    void loadResources();
    void setFullscreen();
};

#endif // APPLICATION_HPP
