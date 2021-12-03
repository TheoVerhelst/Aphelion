#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <PhysicalModel.hpp>
#include <TGUI/TGUI.hpp>

class Application {
public:
    Application(const std::string& setupFile);
    void buildGui();
    void run();

private:
    const std::string _guiFile{"gui.txt"};
    sf::RenderWindow _window;
    tgui::Gui _gui;
    PhysicalModel _model;
    tgui::CanvasSFML::Ptr _simulationCanvas;
};

#endif // APPLICATION_HPP
