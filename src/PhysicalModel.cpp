#include <PhysicalModel.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <json.hpp>

using nlohmann::json;

PhysicalModel::PhysicalModel(const std::string& setupFile) {
	std::ifstream file{setupFile};
	json j;
	try	{
	    j = json::parse(file);
	} catch (json::parse_error& ex) {
	    std::cerr << "parse error in " << setupFile << " at byte " << ex.byte << std::endl;
	}
	for (auto value : j.at("circles")) {
		std::shared_ptr<CircleBody> body{new CircleBody()};
		value.get_to(*body);
		_bodies.push_back(body);
	}
	for (auto value : j.at("polygons")) {
		std::shared_ptr<PolygonBody> body{new PolygonBody()};
		value.get_to(*body);
		_bodies.push_back(body);
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
	while (_currentStep >= _timeStep) {
		_currentStep -= _timeStep;
		updateStep(_timeScale < 0);
	}
}

void PhysicalModel::updateSteps(int steps) {
	bool backwards{steps < 0};
	for (int i{0}; i < std::abs(steps); ++i) {
		updateStep(backwards);
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

std::vector<std::weak_ptr<const Body>> PhysicalModel::getBodies() const {
	return std::vector<std::weak_ptr<const Body>>(_bodies.begin(), _bodies.end());
}

Vector2d PhysicalModel::computeAcceleration(Vector2d position, std::size_t bodyIndex) const {
	Vector2d res{0., 0.};
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		if (i != bodyIndex) {
			Vector2d dx{_bodies[i]->position - position};
			double dist{norm(dx)};
			res += _bodies[i]->mass * dx / (dist * dist * dist);
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
		Vector2d vel{_bodies[i]->velocity};
		Vector2d pos{_bodies[i]->position};
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
		_bodies[i]->position += dx[i];
		_bodies[i]->velocity += dv[i];
	}

	// Check for collisions
	for (std::size_t i{0}; i < _bodies.size(); ++i) {
		for (std::size_t j{i + 1}; j < _bodies.size(); ++j) {
			_bodies[i]->collide(*_bodies[j]);
		}
	}
}
