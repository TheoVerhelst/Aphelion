#include <fstream>
#include <iostream>
#include <sstream>
#include <PhysicalModel.hpp>
#include <helpers.hpp>

PhysicalModel::PhysicalModel(const std::string& filename) {
	std::ifstream file{filename};
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] != '#') {
			double mass, radius, p_x, p_y, v_x, v_y;
			unsigned char r, g, b;
			std::stringstream lineStream{line};
			lineStream >> mass >> radius >> p_x >> p_y >> v_x >> v_y >> r >> g >> b;

			Body body{mass, radius, {p_x, p_y}, {v_x, v_y}};
			_bodies.push_back(body);
			sf::CircleShape circle{static_cast<float>(radius) * _pixelsByMeter};
			circle.setFillColor(sf::Color(r, g, b));
			_circles.push_back(circle);
		}
	}
	_limitRectangle.setPosition({static_cast<float>(_limitOrigin.x), static_cast<float>(_limitOrigin.y)});
	_limitRectangle.setFillColor(sf::Color::Transparent);
	_limitRectangle.setOutlineColor(sf::Color::Red);
	_limitRectangle.setOutlineThickness(2);
}

Vector2d PhysicalModel::computeAcceleration(Vector2d position, std::size_t bodyIndex) const {
	Vector2d res{0., 0.};
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		if (i != bodyIndex) {
			Vector2d difference{_bodies[i].position - position};
			double distance{norm(difference)};
			res += _bodies[i].mass * difference / (distance * distance * distance);
		}
	}

	return res * _gravitationalConstant;
}

void PhysicalModel::update(const sf::Time& elapsedTime) {
	double dt{elapsedTime.asSeconds() * _timeScale};
	std::vector<Vector2d> dv(_bodies.size());
	std::vector<Vector2d> dx(_bodies.size());
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		Vector2d vel{_bodies[i].velocity};
		Vector2d pos{_bodies[i].position};
		Vector2d l1{dt * computeAcceleration(pos, i)};
		Vector2d k1{dt * vel};
		Vector2d l2{dt * computeAcceleration(pos + 0.5 * k1, i)};
		Vector2d k2{dt * (vel + 0.5 * l1)};
		Vector2d l3{dt * computeAcceleration(pos + 0.5 * k2, i)};
		Vector2d k3{dt * (vel + 0.5 * l2)};
		Vector2d l4{dt * computeAcceleration(pos + k3, i)};
		Vector2d k4{dt * (vel + l3)};
		dx[i] = (k1 + 2. * k2 + 2. *  k3 + k4) / 6.;
		dv[i] = (l1 + 2. * l2 + 2. *  l3 + l4) / 6.;
	}

	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		_bodies[i].position += dx[i];
		_bodies[i].velocity += dv[i];
		// Keep ball in limit box, pacman style. Note that the gravity geomety
		// does not follow this topology.
		_bodies[i].position.x = wrap(_bodies[i].position.x - _limitOrigin.x, _limitSize.x) + _limitOrigin.x;
		_bodies[i].position.y = wrap(_bodies[i].position.y - _limitOrigin.y, _limitSize.y) + _limitOrigin.y;
		_circles[i].setPosition(static_cast<sf::Vector2f>(_bodies[i].position) * _pixelsByMeter);
	}
	std::cout << std::endl;
}

void PhysicalModel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& circle : _circles) {
		target.draw(circle, states);
	}
	target.draw(_limitRectangle, states);
}

void PhysicalModel::setPixelsByMeter(float pixelsByMeter) {
	_pixelsByMeter = pixelsByMeter;
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		_circles[i].setRadius(_bodies[i].radius * _pixelsByMeter);
	}
}

void PhysicalModel::setTimeScale(double timeScale) {
	_timeScale = timeScale;
}
