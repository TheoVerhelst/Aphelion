#include <Application.hpp>
#include <format.hpp>

Application::Application(const std::string& setupFile):
        _window{sf::VideoMode(1200, 600), "Physics Simulation"},
        _gui{_window},
        _model{setupFile} {
    buildGui();
}

void Application::run() {
    sf::Clock clock;

    while (_window.isOpen()) {
        // Update
        sf::Time elapsedTime{clock.restart()};
        _model.updateTime(elapsedTime);
        updateDisplays();

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

void Application::buildGui() {
	_gui.loadWidgetsFromFile(_guiFile);

    // Spin controls. Do not work with GUI text file import for some reason.
    tgui::SpinControl::Ptr timeSpeedControl = tgui::SpinControl::create(-10, 10, 1, 2, 0.1);
    timeSpeedControl->setPosition(100, 82);
    timeSpeedControl->setSize(124, 16);
    timeSpeedControl->onValueChange([this](float value){_model.setTimeScale(value);});
    _gui.get<tgui::Panel>("controlsPanel")->add(timeSpeedControl, "timeSpeedcontrol");

    tgui::SpinControl::Ptr spaceSizeControl = tgui::SpinControl::create(0, 10, 1, 2, 0.1);
    spaceSizeControl->setPosition(100, 20);
    spaceSizeControl->setSize(124, 16);
    spaceSizeControl->onValueChange([this](float value){_model.setPixelsByMeter(value);});
    _gui.get<tgui::Panel>("controlsPanel")->add(spaceSizeControl);

    // Other bindings
    _gui.get<tgui::Button>("stepBackButton")->onPress([this, &timeSpeedControl] () {
        //timeSpeedControl->setValue(0);
        _model.updateSteps(-1);
    });
    _gui.get<tgui::Button>("stepForwardButton")->onPress([this, &timeSpeedControl] () {
        //timeSpeedControl->setValue(0);
        _model.updateSteps(1);
    });
    _gui.get<tgui::Button>("pauseButton")->onPress([this, &timeSpeedControl] () {
        //timeSpeedControl->setValue(0);
    });

    // Simulation canvas
    _simulationCanvas = tgui::CanvasSFML::create();
    _simulationCanvas->setPosition(0, 0);
    _simulationCanvas->setSize({"100%", "100%"});
    _gui.add(_simulationCanvas, "simulationCanvas");
    _simulationCanvas->moveToBack();
}

void Application::updateDisplays() {
    _gui.get<tgui::Label>("timeSecondsDisplay")->setText(formatTime(_model.getElapsedTime()));
    _gui.get<tgui::Label>("timeStepsDisplay")->setText(std::to_string(_model.getStepCounter()));
}
