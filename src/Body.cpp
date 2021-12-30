#include <algorithm>
#include <functional>
#include <cmath>
#include <cstddef>
#include <Body.hpp>
#include <collisions.hpp>


Body::Body(double mass, const Vector2d& position, const Vector2d& velocity,
		const sf::Color color, double angularVelocity):
    _mass{mass},
    _position{position},
    _velocity{velocity},
    _color{color},
    _angularVelocity{angularVelocity} {
}

double Body::getMass() const {
    return _mass;
}

const Vector2d& Body::getPosition() const {
    return _position;
}

const Vector2d& Body::getVelocity() const {
    return _velocity;
}

const sf::Color& Body::getColor() const {
    return _color;
}

const double& Body::getMomentOfInertia() const {
    return _momentOfInertia;
}

const double& Body::getAngularVelocity() const {
    return _angularVelocity;
}

void Body::setMass(double mass) {
    _mass = mass;
}

void Body::setPosition(const Vector2d& position) {
    _position = position;
}

void Body::setVelocity(const Vector2d& velocity) {
    _velocity = velocity;
}

void Body::setColor(const sf::Color& color) {
    _color = color;
}

void Body::setAngularVelocity(const double& angularVelocity) {
    _angularVelocity = angularVelocity;
}

void Body::setMomentOfInertia(double momentOfInertia) {
	_momentOfInertia = momentOfInertia;
}

CircleBody::CircleBody(double mass, const Vector2d& position, const Vector2d& velocity,
        const sf::Color color, double angularVelocity, double radius):
    Body(mass, position, velocity, color, angularVelocity),
    _radius{radius} {
    setMomentOfInertia(_mass * _radius * _radius / 2);
}


void CircleBody::collide(Body& other) {
    other.collide(*this);
}

void CircleBody::collide(CircleBody& other) {
    collideCircles(*this, other);
}

void CircleBody::collide(PolygonBody& other) {
    collideBodies(*this, other);
}

std::shared_ptr<sf::Shape> CircleBody::createShape() const {
    std::shared_ptr<sf::CircleShape> circle{new sf::CircleShape(static_cast<float>(_radius))};
    circle->setOrigin(_radius, _radius);
    circle->setFillColor(_color);
    return circle;
}

double CircleBody::getRadius() const {
	return _radius;
}

Vector2d CircleBody::support(Vector2d direction) const {
    return _position + direction * _radius / norm(direction);
}

PolygonBody::PolygonBody(double mass, const Vector2d& position, const Vector2d& velocity,
	   const sf::Color color, double angularVelocity, const std::vector<Vector2d>& vertices):
   Body(mass, position, velocity, color, angularVelocity),
    _vertices{vertices} {
    setMomentOfInertia(computeMomentOfInertia());
}

void PolygonBody::collide(Body& other) {
    other.collide(*this);
}

void PolygonBody::collide(CircleBody& other) {
    other.collide(*this);
}

void PolygonBody::collide(PolygonBody& other) {
    collideBodies(*this, other);
}

std::shared_ptr<sf::Shape> PolygonBody::createShape() const {
    std::shared_ptr<sf::ConvexShape> polygon{new sf::ConvexShape(_vertices.size())};
    for(std::size_t i{0}; i < _vertices.size(); ++i) {
        polygon->setPoint(i, static_cast<Vector2f>(_vertices[i]));
    }
    polygon->setFillColor(_color);
    return polygon;
}

const std::vector<Vector2d>& PolygonBody::getVertices() const {
	return _vertices;
}

Vector2d PolygonBody::support(Vector2d direction) const {
    std::vector<double> products(_vertices.size());
    // Compute all dot products between vertices and direction
    std::transform(
        _vertices.begin(),
        _vertices.end(),
        products.begin(),
        std::bind(dot<double>, std::placeholders::_1, direction)
    );
    // Find the largest one
    auto largest{std::distance(
        products.begin(),
        std::max_element(products.begin(), products.end())
    )};
    return _vertices[largest] + _position;
}

double PolygonBody::computeMomentOfInertia() const {
    std::vector<double> areas;
    std::vector<Vector2d> centersOfMass;
    for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
        Vector2d B{_vertices[i] - _vertices[0]};
        Vector2d C{_vertices[i+1] - _vertices[0]};
        areas.push_back(std::abs(cross(B, C))/2);
        centersOfMass.push_back((B + C) / 3.);
    }
    double totalArea{std::accumulate(areas.begin(), areas.end(), 0.)};
    Vector2d totalCenterOfMass{0, 0};
    double momentOfInertia{0};
    for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
        Vector2d B{_vertices[i] - _vertices[0]};
        Vector2d C{_vertices[i + 1] - _vertices[0]};
        double signedArea{cross(B, C)/2.};
        double area{areas[i - 1]};
        double mass{_mass * area / totalArea};
        totalCenterOfMass += centersOfMass[i] * mass;
        momentOfInertia += mass * (signedArea / area) * (norm2(B) + norm2(C) + dot(B, C));
    }
    totalCenterOfMass /= _mass;

    return (momentOfInertia / 6) - _mass * norm2(_vertices[0] - totalCenterOfMass);
}
