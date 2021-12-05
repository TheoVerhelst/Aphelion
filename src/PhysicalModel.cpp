#include <fstream>
#include <cmath>
#include <iostream>
#include <sstream>
#include <PhysicalModel.hpp>
#include <helpers.hpp>

PhysicalModel::PhysicalModel(const std::string& setupFile) {
	std::ifstream file{setupFile};
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] != '#') {
			double mass, radius, p_x, p_y, v_x, v_y;
			unsigned int r, g, b;
			std::stringstream lineStream{line};
			lineStream >> mass >> radius >> p_x >> p_y >> v_x >> v_y >> r >> g >> b;

			Body body{mass, radius, {p_x, p_y}, {v_x, v_y}};
			_bodies.push_back(body);
			sf::CircleShape circle{static_cast<float>(radius) * _pixelsByMeter};
			circle.setFillColor(sf::Color(r, g, b));
			_circles.push_back(circle);
		}
	}
}

Vector2d PhysicalModel::computeAcceleration(Vector2d position, std::size_t bodyIndex) const {
	Vector2d res{0., 0.};
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		if (i != bodyIndex) {
			Vector2d dx{_bodies[i].position - position};
			double dist{norm(dx)};
			res += _bodies[i].mass * dx / (dist * dist * dist);
		}
	}
	return res * _gravitationalConstant;
}

void PhysicalModel::update(const sf::Time& elapsedTime) {
	// Skip physics update until we waited for enough time (_timeStep)
	_currentStep += elapsedTime  * _timeScale;
	if (_currentStep < _timeStep) {
		return;
	}

	while(_currentStep >= _timeStep) {
		_currentStep -= _timeStep;

		double dt{_timeStep.asSeconds()};
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
			_circles[i].setPosition(static_cast<sf::Vector2f>(_bodies[i].position) * _pixelsByMeter);
		}

		// Check for collisions
		for (std::size_t i{0}; i < _bodies.size(); ++i) {
			for (std::size_t j{i + 1}; j < _bodies.size(); ++j) {
				const Vector2d dpos = _bodies[j].position - _bodies[i].position;
				const double dist{norm(dpos)};
				const double overlap{_bodies[i].radius + _bodies[j].radius - dist};

				// Collision
				if (overlap > 0) {
					// Let's use an inertial frame where j has null velocity
					const Vector2d v_i = _bodies[i].velocity - _bodies[j].velocity;
					const double m_i = _bodies[i].mass;
					const double m_j = _bodies[j].mass;
					const double angle_frame = angle(v_i);
					const double angle_rebound_j = angle(dpos);
					const double s_i = norm(v_i);
					const double theta_j = angle_rebound_j - angle_frame;
					const double new_s_j = 2 * s_i * std::cos(theta_j) * m_i * m_j
							/ (m_i * m_i + m_j * m_j);
					const double theta_i = std::atan2(
						m_j * new_s_j * std::sin(theta_j),
						m_j * new_s_j * std::cos(theta_j) - m_i * s_i
					);
					const double new_s_i = m_j * new_s_j * std::sin(theta_j) / (m_i * std::sin(theta_i));

					// const double pi{3.14159265};
					// std::cout << "or_v_i = [" << _bodies[i].velocity.x << ", " << _bodies[i].velocity.y << "]" << std::endl;
					// std::cout << "or_v_j = [" << _bodies[j].velocity.x << ", " << _bodies[j].velocity.y << "]" << std::endl;
					// std::cout << "or_s_i = " << norm(_bodies[i].velocity) << std::endl;
					// std::cout << "or_s_j = " << norm(_bodies[j].velocity) << std::endl;
					// std::cout << "or_theta_i = " << angle(_bodies[i].velocity) * 180 / pi << std::endl;
					// std::cout << "or_theta_j = " << angle(_bodies[j].velocity) * 180 / pi << std::endl;
					// std::cout << "dpos = [" << dpos.x << ", " << dpos.y << "]" << std::endl;
					// std::cout << "v_i = [" << v_i.x << ", " << v_i.y << "]" << std::endl;
					// std::cout << "s_i = " << s_i << std::endl;
					// std::cout << "angle_frame = " << angle_frame * 180 / pi << std::endl;
					// std::cout << "angle_rebound_j = " << angle_rebound_j * 180 / pi << std::endl;
					// std::cout << "theta_i = " << theta_i * 180 / pi << std::endl;
					// std::cout << "theta_j = " << theta_j * 180 / pi << std::endl;
					// std::cout << "new_s_i = " << new_s_i << std::endl;
					// std::cout << "new_s_j = " << new_s_j << std::endl;
					// std::cout << "-------------------" << std::endl;

					_bodies[i].velocity = Vector2d(new_s_i * std::cos(theta_i), new_s_i * std::sin(theta_i)) + _bodies[j].velocity;
					_bodies[j].velocity = Vector2d(new_s_j * std::cos(theta_j), new_s_j * std::sin(theta_j)) + _bodies[j].velocity;

					// Move the bodies so that they just touch and don't overlap
					_bodies[i].position -= m_j * overlap * dpos / (dist * (m_i + m_j));
					_bodies[j].position += m_i * overlap * dpos / (dist * (m_i + m_j));

				}
			}
		}
	}
	// Update the graphic objects only once
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		_circles[i].setPosition(static_cast<sf::Vector2f>(_bodies[i].position) * _pixelsByMeter);
	}
}

void PhysicalModel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& circle : _circles) {
		target.draw(circle, states);
	}
}

void PhysicalModel::setPixelsByMeter(float pixelsByMeter) {
	_pixelsByMeter = pixelsByMeter;
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		_circles[i].setRadius(_bodies[i].radius * _pixelsByMeter);
		// Position update will be done next tick, no need to do it here
	}
}

void PhysicalModel::setTimeScale(float timeScale) {
	_timeScale = timeScale;
}
