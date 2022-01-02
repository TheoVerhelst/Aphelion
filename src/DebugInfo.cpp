#include <cmath>
#include <iostream>
#include <DebugInfo.hpp>

DebugInfo::DebugInfo() {
    if(not _font.loadFromFile("resources/FreeSans.ttf")) {
        throw std::runtime_error("Error loading font");
    }
    _position.setOrigin(_position.getRadius(), _position.getRadius());
    _rotation.setCharacterSize(10);
    _angularVelocity.setCharacterSize(10);
}

void DebugInfo::update(const Body& body) {
    Vector2f pos{static_cast<Vector2f>(body.getPosition())};
    _position.setPosition(pos);
    sf::Color color{body.getColor()};
    // Parenthesis initializer to allow implicit conversion to sf::Uint8
    sf::Color opposite(127 - color.r, 127 - color.g, 127 - color.b, color.a);
    _position.setFillColor(opposite);

    std::stringstream rotationStr, angularVelocityStr;
    rotationStr        << "r = " << std::fixed << std::setprecision(2) << body.getRotation() << " rad";
    angularVelocityStr << "w = " << std::fixed << std::setprecision(2) << body.getAngularVelocity() << " rad/s";
    _rotation.setString(rotationStr.str());
    _angularVelocity.setString(angularVelocityStr.str());
    _rotation.setPosition(pos + Vector2f(10, -31));
    _angularVelocity.setPosition(pos + Vector2f(10, -20));

    Vector2d com{body.getPosition() + body.getCenterOfMass()};
    // Use the square root of the velocity to avoid huge arrows
    Vector2d vel{body.getVelocity() / std::sqrt(norm(body.getVelocity()))};
    _velocity.setPoints(static_cast<Vector2f>(com), static_cast<Vector2f>(com + vel));
}

void DebugInfo::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    
    target.draw(_position, states);
    target.draw(_rotation, states);
    target.draw(_angularVelocity, states);
    target.draw(_velocity, states);
}

void DebugInfo::ArrowShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_arrow, states);
}

void DebugInfo::ArrowShape::setPoints(const Vector2f& begin, const Vector2f& end) {
    _arrow[0] = begin;
    _arrow[1] = end;
    Vector2f headUp{-std::cos(_headAngle), std::sin(_headAngle)};
    Vector2f headDown{headUp.x, -headUp.y};
    float arrowAngle{std::atan2(end.y - begin.y, end.x - begin.x)};
    _arrow[2] = end;
    _arrow[3] = end + rotate(headUp, arrowAngle) * _headLength;
    _arrow[4] = end;
    _arrow[5] = end + rotate(headDown, arrowAngle) * _headLength;
}
