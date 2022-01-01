#include <algorithm>
#include <functional>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <Body.hpp>
#include <collisions.hpp>


Body::Body(double mass, const Vector2d& position, const Vector2d& velocity,
		double rotation, double angularVelocity, const sf::Color color):
    _mass{mass},
    _position{position},
    _velocity{velocity},
    _rotation{rotation},
    _angularVelocity{angularVelocity},
    _color{color} {
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

double Body::getRotation() const {
    return _rotation;
}

double Body::getAngularVelocity() const {
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

void Body::setRotation(double rotation) {
    _rotation = rotation;
}

void Body::setAngularVelocity(double angularVelocity) {
    _angularVelocity = angularVelocity;
}

CircleBody::CircleBody(double mass, const Vector2d& position, const Vector2d& velocity,
        double rotation, double angularVelocity, const sf::Color color, double radius):
    Body(mass, position, velocity, rotation, angularVelocity, color),
    _radius{radius} {
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

double CircleBody::getMomentOfInertia() const {
	return _mass * _radius * _radius / 2;
}

Vector2d CircleBody::getCenterOfMass() const {
	return {0, 0};
}

double CircleBody::getRadius() const {
	return _radius;
}

Vector2d CircleBody::support(const Vector2d& direction) const {
    return _position + direction * _radius / norm(direction);
}

PolygonBody::PolygonBody(double mass, const Vector2d& position, const Vector2d& velocity,
	   double rotation, double angularVelocity, const sf::Color color,
	   const std::vector<Vector2d>& vertices):
	Body(mass, position, velocity, rotation, angularVelocity, color),
	_vertices{vertices},
	_centerOfMass{computeCenterOfMass()},
	_momentOfInertia{computeMomentOfInertia()} {
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

Vector2d PolygonBody::getCenterOfMass() const {
	return _centerOfMass;
}

double PolygonBody::getMomentOfInertia() const {
	return _momentOfInertia;
}

const std::vector<Vector2d>& PolygonBody::getVertices() const {
	return _vertices;
}

Vector2d PolygonBody::support(const Vector2d& direction) const {
	double largest{-std::numeric_limits<double>::max()};
	Vector2d furthest;
	for (Vector2d vertex : _vertices) {
		// Rotate the point around the center of mass to account for rotation
		Vector2d rotated{rotate(vertex - getCenterOfMass(), getRotation()) + getCenterOfMass()};
		double product{dot(direction, rotated)};
		if (product > largest) {
			furthest = rotated;
			largest = product;
		}
	}
    return furthest + getPosition();
}

Vector2d PolygonBody::computeCenterOfMass() const {
	if (_vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
	std::vector<double> triangleAreas;
	std::vector<Vector2d> triangleCenters;
	for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
		Vector2d B{_vertices[i] - _vertices[0]};
		Vector2d C{_vertices[i+1] - _vertices[0]};
		triangleAreas.push_back(std::abs(cross(B, C)) / 2.);
		triangleCenters.push_back((B + C) / 3.);
	}
	double totalArea{std::accumulate(triangleAreas.begin(), triangleAreas.end(), 0.)};
	Vector2d centerOfMass{std::inner_product(triangleCenters.begin(),
		triangleCenters.end(), triangleAreas.begin(), Vector2d(0, 0))};
	return centerOfMass / totalArea;
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
    double momentOfInertia{0};
    for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
        Vector2d B{_vertices[i] - _vertices[0]};
        Vector2d C{_vertices[i + 1] - _vertices[0]};
        double signedArea{cross(B, C)/2.};
        double area{areas[i - 1]};
        double mass{_mass * area / totalArea};
        momentOfInertia += mass * (signedArea / area) * (norm2(B) + norm2(C) + dot(B, C));
    }

    return (momentOfInertia / 6) - _mass * norm2(_vertices[0] - getCenterOfMass());
}
