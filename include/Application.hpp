#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <PhysicalModel.hpp>
#include <Scene.hpp>
#include <SimulationCanvas.hpp>
#include <ResourceManager.hpp>

class Application {
public:
    Application(const std::string& setupFile);
    void run();

private:
    const std::string _guiFile{"gui.txt"};
    sf::RenderWindow _window;
    tgui::Gui _gui;
    PhysicalModel _model;
    Scene _scene;
    SimulationCanvas::Ptr _simulationCanvas;
    bool _paused{false};
    ResourceManager<sf::Font> _fontManager;
    ResourceManager<sf::Texture> _textureManager;

    void buildGui();
    void updateDisplays();
    void pauseTime();
    void resumeTime();
};

#endif // APPLICATION_HPP
