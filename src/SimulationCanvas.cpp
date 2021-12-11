#include <SimulationCanvas.hpp>

SimulationCanvas::SimulationCanvas(PhysicalModel& model, tgui::Layout2d size):
    _model{model}
{
    setSize(size);
    onSizeChange(&SimulationCanvas::updateView, this);
}

void SimulationCanvas::updateView() {
    sf::View view{getDefaultView()};
    view.setCenter(_position);
    view.setSize(view.getSize() * _zoom);
    setView(view);
}

void SimulationCanvas::update(const sf::Time& elapsedTime) {
    Vector2f movement{0, 0};
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        movement.x -= _panSpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        movement.x += _panSpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        movement.y -= _panSpeed;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        movement.y += _panSpeed;
    }
    // Normalize the speed to _panSpeed, in case of diagonal movement
    float speed{norm(movement)};
    if(speed > 0) {
        movement = movement * _panSpeed / speed;
    }

    // Increase movement speed when spacebar is pressed
    float multiplier{1};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        multiplier = 4;
    }

    float zoomFactor{1};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
        or sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        zoomFactor *= _zoomSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
        or sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
        zoomFactor /= _zoomSpeed;
    }
    _zoom *= std::pow(zoomFactor, elapsedTime.asSeconds() * multiplier);

    _position += movement * elapsedTime.asSeconds() * multiplier;
    updateView();
}


SimulationCanvas::Ptr SimulationCanvas::create(PhysicalModel& model, tgui::Layout2d size) {
    return std::make_shared<SimulationCanvas>(model, size);
}
