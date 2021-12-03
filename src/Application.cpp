#include <Application.hpp>

Application::Application(const std::string& setupFile):
        _window{sf::VideoMode(1200, 600), "Physics Simulation"},
        _gui{_window},
        _model{setupFile} {
    buildGui();
}

void Application::buildGui() {
	_gui.loadWidgetsFromFile(_guiFile);

    // Event bindings
    _gui.get<tgui::Slider>("timeSlider")->onValueChange([this](float value){_model.setTimeScale(value);});
    _gui.get<tgui::Slider>("spaceSlider")->onValueChange([this](float value){_model.setPixelsByMeter(value);});

    // Simulation canvas
    _simulationCanvas = tgui::CanvasSFML::create();
    _gui.add(_simulationCanvas, "simulationCanvas");
    _simulationCanvas->setPosition(0, 0);
    _simulationCanvas->setSize({"100%", "100%"});
    _simulationCanvas->moveToBack();
}

void Application::run() {
    sf::Clock clock;

    while (_window.isOpen()) {
        // Update
        sf::Time elapsedTime{clock.restart()};
        _model.update(elapsedTime);

        // Handle events
        sf::Event event;
        while (_window.pollEvent(event)) {
            // Pass the event to all the widgets
            _gui.handleEvent(event);

            // When the window is closed, the application ends
            if (event.type == sf::Event::Closed)
                _window.close();

            // When the window is resized, the view is changed
            else if (event.type == sf::Event::Resized) {
                _window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));
            }
        }

        // Draw graphics
        _window.clear(sf::Color::White);
        _simulationCanvas->clear(sf::Color::Black);
        _simulationCanvas->draw(_model);
        _simulationCanvas->display();
        _gui.draw();
        _window.display();
    }
}
