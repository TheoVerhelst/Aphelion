#include <PhysicalModel.hpp>
#include <TGUI/TGUI.hpp>

void buildGui(tgui::Gui& gui, PhysicalModel& model, const std::string& guiFile);

int main(int argc, char** argv) {
    std::string setupFile{"setup.txt"};
    if(argc >= 2) {
        setupFile = argv[1];
    }

    // Init main objects
    sf::Clock clock;
    sf::RenderWindow window{sf::VideoMode(1200, 600), "Physics Simulation"};
    tgui::Gui gui{window};
    PhysicalModel model{setupFile};
    buildGui(gui, model, "gui.txt");
    tgui::CanvasSFML::Ptr simulationCanvas{gui.get<tgui::CanvasSFML>("simulationCanvas")};

    // Main loop
    while (window.isOpen()) {
        // Update
        sf::Time elapsedTime{clock.restart()};
        model.update(elapsedTime);

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Pass the event to all the widgets
            gui.handleEvent(event);

            // When the window is closed, the application ends
            if (event.type == sf::Event::Closed)
                window.close();

            // When the window is resized, the view is changed
            else if (event.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));
            }
        }

        // Draw graphics
        window.clear(sf::Color::White);
        simulationCanvas->clear(sf::Color::Black);
        simulationCanvas->draw(model);
        simulationCanvas->display();
        gui.draw();
        window.display();
    }

    return EXIT_SUCCESS;
}

void buildGui(tgui::Gui& gui, PhysicalModel& model, const std::string& guiFile) {
	gui.loadWidgetsFromFile(guiFile);

    // Event bindings
    gui.get<tgui::Slider>("timeSlider")->onValueChange([&model](float value){model.setTimeScale(value);});
    gui.get<tgui::Slider>("spaceSlider")->onValueChange([&model](float value){model.setPixelsByMeter(value);});

    // Simulation canvas
    tgui::CanvasSFML::Ptr simulationCanvas = tgui::CanvasSFML::create();
    gui.add(simulationCanvas, "simulationCanvas");
    simulationCanvas->setPosition(0, 0);
    simulationCanvas->setSize({"100%", "100%"});
    simulationCanvas->moveToBack();
}
