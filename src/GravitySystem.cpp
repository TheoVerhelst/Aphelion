#include <GravitySystem.hpp>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

GravitySystem::GravitySystem(Scene& scene):
    _scene{scene},
    _collisionSystem{_scene} {
}

void GravitySystem::updateTime(const sf::Time& elapsedTime) {
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

void GravitySystem::updateSteps(int steps) {
	bool backwards{steps < 0};
	for (int i{0}; i < std::abs(steps); ++i) {
		updateStep(backwards);
	}
}

void GravitySystem::setTimeScale(float timeScale) {
	_timeScale = timeScale;
}

long long int GravitySystem::getStepCounter() const {
	return _stepCounter;
}

sf::Time GravitySystem::getElapsedTime() const {
	return _timeStep * _stepCounter;
}

Vector2d GravitySystem::computeAcceleration(const Vector2d& position, EntityId id) const {
	Vector2d res{0., 0.};
	for(EntityId otherId : _scene.view<Body>()) {
		if (otherId != id) {
			const Body& otherBody{_scene.getComponent<Body>(otherId)};
			Vector2d dx{otherBody.position - position};
			double dist{norm(dx)};
			res += otherBody.mass * dx / (dist * dist * dist);
		}
	}
	return res * _gravitationalConstant;
}

void GravitySystem::updateStep(bool backwards) {
	_stepCounter += backwards ? -1 : 1;

	double dt{_timeStep.asSeconds() * (backwards ? -1 : 1)};
	std::map<EntityId, Vector2d> dv;
	std::map<EntityId, Vector2d> dx;
	for(EntityId id : _scene.view<Body>()) {
		Body& body{_scene.getComponent<Body>(id)};
		Vector2d vel{body.velocity};
		Vector2d pos{body.position};
		Vector2d l1{dt * computeAcceleration(pos, id)};
		Vector2d k1{dt * vel};
		Vector2d l2{dt * computeAcceleration(pos + 0.5 * k1, id)};
		Vector2d k2{dt * (vel + 0.5 * l1)};
		Vector2d l3{dt * computeAcceleration(pos + 0.5 * k2, id)};
		Vector2d k3{dt * (vel + 0.5 * l2)};
		Vector2d l4{dt * computeAcceleration(pos + k3, id)};
		Vector2d k4{dt * (vel + l3)};
		dx[id] = (k1 + 2. * k2 + 2. *  k3 + k4) / 6.;
		dv[id] = (l1 + 2. * l2 + 2. *  l3 + l4) / 6.;
	}

	for(EntityId id : _scene.view<Body>()) {
		Body& body{_scene.getComponent<Body>(id)};
		body.position += dx[id];
		body.velocity += dv[id];
		body.rotation += body.angularVelocity * dt;
	}
    _collisionSystem.update();
}
