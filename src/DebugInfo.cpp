#include <cmath>
#include <limits>
#include <DebugInfo.hpp>

DebugInfo::DebugInfo(const sf::Font& font) {
    _position.setOrigin(_position.getRadius(), _position.getRadius());
    _rotation.setFont(font);
    _rotation.setCharacterSize(10);
    _angularVelocity.setFont(font);
    _angularVelocity.setCharacterSize(10);
}

void DebugInfo::update(const Body& body) {
    Vector2f pos{static_cast<Vector2f>(body.position)};
    _position.setPosition(pos);
    _position.setFillColor(sf::Color::White);

    std::stringstream rotationStr, angularVelocityStr;
    rotationStr        << "r = " << std::fixed << std::setprecision(2) << body.rotation << " rad";
    angularVelocityStr << "w = " << std::fixed << std::setprecision(2) << body.angularVelocity << " rad/s";
    _rotation.setString(rotationStr.str());
    _angularVelocity.setString(angularVelocityStr.str());
    _rotation.setPosition(pos + Vector2f(10, -31));
    _angularVelocity.setPosition(pos + Vector2f(10, -20));

    _velocity.setPoints(static_cast<Vector2f>(body.position), static_cast<Vector2f>(body.position + body.velocity));
    _velocity.setColor(sf::Color::White);
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

void DebugInfo::ArrowShape::setColor(const sf::Color& color) {
    for (std::size_t i{0}; i < _arrow.getVertexCount(); ++i) {
        _arrow[i].color = color;
    }
}
