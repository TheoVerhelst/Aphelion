#include <SimulationCanvas.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

SimulationCanvas::SimulationCanvas(const char* typeName, bool initRenderer):
        tgui::CanvasSFML(typeName, initRenderer) {
    onSizeChange(&SimulationCanvas::updateView, this);
}

SimulationCanvas::Ptr SimulationCanvas::create(tgui::Layout2d size) {
    auto canvas = std::make_shared<SimulationCanvas>();
    canvas->setSize(size);
    return canvas;
}

SimulationCanvas::Ptr SimulationCanvas::copy(SimulationCanvas::ConstPtr widget) {
    if (widget) {
        return std::static_pointer_cast<SimulationCanvas>(widget->clone());
    } else {
        return nullptr;
    }
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

    float zoomFactor{1};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
        or sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        zoomFactor *= _zoomSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
        or sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
        zoomFactor /= _zoomSpeed;
    }

    // Increase movement speed when spacebar is pressed
    float multiplier{1};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        multiplier = 4;
    }

    _zoom *= std::pow(zoomFactor, elapsedTime.asSeconds() * multiplier);
    _position += movement * elapsedTime.asSeconds() * multiplier;
    updateView();
}

tgui::Widget::Ptr SimulationCanvas::clone() const {
    return std::make_shared<SimulationCanvas>(*this);
}

void SimulationCanvas::updateView() {
    sf::View view{getDefaultView()};
    view.setCenter(_position);
    view.setSize(view.getSize() * _zoom);
    setView(view);
}
