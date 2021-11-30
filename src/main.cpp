#include <PhysicalModel.hpp>
#include <TGUI/TGUI.hpp>

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FILE" << std::endl;
        return 1;
    }

    // Init main objects
    PhysicalModel model{argv[1]};
    sf::RenderWindow window{sf::VideoMode(1200, 600), "Physics Simulation"};
    tgui::Gui gui{window};
    sf::Clock clock;

	//gui.loadWidgetsFromFile("gui.txt");
    //gui.get("timeScaleSlider")->connect("ValueChanged", [&model](float value){model.setTimeScale(value);});
    //gui.get("pixelsByMeterSlider")->connect("ValueChanged", [&model](float value){model.setPixelsByMeter(value);});

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
        gui.draw();
        window.draw(model);
        window.display();
    }

    return EXIT_SUCCESS;
}
