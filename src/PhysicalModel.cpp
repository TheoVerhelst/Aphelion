#include <PhysicalModel.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

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
			sf::CircleShape circle{static_cast<float>(radius)};
			circle.setFillColor(sf::Color(r, g, b));
			_circles.push_back(circle);
		}
	}
}

void PhysicalModel::updateTime(const sf::Time& elapsedTime) {
	// Skip physics update until we waited for enough time (which is _timeStep)
	// We take into account the time scale here. If it is negative, we proceed
	// as if it was positive but we specify to the update function to go backwards.
	_currentStep += elapsedTime * std::abs(_timeScale);
	if (_currentStep < _timeStep) {
		return;
	}

	// Update until we get back under _timestep
	// Usually only one update will be needed
	while(_currentStep >= _timeStep) {
		_currentStep -= _timeStep;
		updateStep(_timeScale < 0);
	}

	updateGraphics();
}

void PhysicalModel::updateSteps(int steps) {
	bool backwards{steps < 0};
	for (int i{0}; i < std::abs(steps); ++i) {
		updateStep(backwards);
	}

	updateGraphics();
}

void PhysicalModel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& circle : _circles) {
		target.draw(circle, states);
	}
}

void PhysicalModel::setTimeScale(float timeScale) {
	_timeScale = timeScale;
}

long long int PhysicalModel::getStepCounter() const {
	return _stepCounter;
}

sf::Time PhysicalModel::getElapsedTime() const {
	return _timeStep * _stepCounter;
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

void PhysicalModel::updateStep(bool backwards) {
	_stepCounter += backwards ? -1 : 1;

	double dt{_timeStep.asSeconds() * (backwards ? -1 : 1)};
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
	}

	// Check for collisions
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		for (std::size_t j{i + 1}; j < _bodies.size(); ++j) {
			const Vector2d diff_x = _bodies[j].position - _bodies[i].position;
			const double dist{norm(diff_x)};
			const double overlap{_bodies[i].radius + _bodies[j].radius - dist};

			// Collision
			if (overlap > 0) {
				// Distance squared
				const double dist_2{dot(diff_x, diff_x)};
				const Vector2d v_i = _bodies[i].velocity;
				const Vector2d v_j = _bodies[j].velocity;
				const double m_i = _bodies[i].mass;
				const double m_j = _bodies[j].mass;
				// Based on stackoverflow.com/questions/35211114/2d-elastic-ball-collision-physics
				const Vector2d addedVel = 2 * dot(v_i - v_j, diff_x) * diff_x / ((m_i + m_j) * dist_2);
				_bodies[i].velocity = v_i - addedVel * m_j;
				_bodies[j].velocity = v_j + addedVel * m_i;

				// Move the bodies so that they just touch and don't overlap.
				// The displacement is proportional to the mass of the other body.
				_bodies[i].position -= m_j * overlap * diff_x / (dist * (m_i + m_j));
				_bodies[j].position += m_i * overlap * diff_x / (dist * (m_i + m_j));
			}
		}
	}
}

void PhysicalModel::updateGraphics() {
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		const Vector2d r{_bodies[i].radius, _bodies[i].radius};
		_circles[i].setPosition(static_cast<sf::Vector2f>(_bodies[i].position - r));
	}
}
