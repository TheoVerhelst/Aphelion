#include <cmath>
#include <systems/PhysicsSystem.hpp>
#include <Scene.hpp>
#include <components.hpp>

PhysicsSystem::PhysicsSystem(Scene& scene):
    _scene{scene} {
}

void PhysicsSystem::update(sf::Time dt) {
	// Skip physics update until we waited for enough time (which is _timeStep)
	// We take into account the time scale here. If it is negative, we proceed
	// as if it was positive but we specify to the update function to go
	// backwards.
	_currentStep += dt * std::abs(_timeScale);
	if (_currentStep < _timeStep) {
		return;
	}

	// Update until we get back under _timestep
	// Usually only one update will be needed
	while (_currentStep >= _timeStep) {
		_currentStep -= _timeStep;
		updateStep(_timeScale < 0.f);
	}
}

void PhysicsSystem::updateSteps(int steps) {
	bool backwards{steps < 0};
	for (int i{0}; i < std::abs(steps); ++i) {
		updateStep(backwards);
	}
}

void PhysicsSystem::setTimeScale(float timeScale) {
	_timeScale = timeScale;
}

long long int PhysicsSystem::getStepCounter() const {
	return _stepCounter;
}

sf::Time PhysicsSystem::getElapsedTime() const {
	return _timeStep * _stepCounter;
}

Vector2f PhysicsSystem::computeAcceleration(const Vector2f& position, EntityId id) const {
	Vector2f res{0.f, 0.f};
	for(EntityId otherId : _scene.view<Body>()) {
		if (otherId != id) {
			const Body& otherBody{_scene.getComponent<Body>(otherId)};
			Vector2f dx{otherBody.position - position};
			float dist{norm(dx)};
			res += otherBody.mass * dx / (dist * dist * dist);
		}
	}
	return res * _gravitationalConstant;
}

void PhysicsSystem::updateStep(bool backwards) {
	_stepCounter += backwards ? -1 : 1;

	float dt{_timeStep.asSeconds() * (backwards ? -1.f : 1.f)};
	std::map<EntityId, Vector2f> dv;
	std::map<EntityId, Vector2f> dx;
	for(EntityId id : _scene.view<Body>()) {
		Body& body{_scene.getComponent<Body>(id)};
		Vector2f vel{body.velocity};
		Vector2f pos{body.position};
		Vector2f l1{dt * computeAcceleration(pos, id)};
		Vector2f k1{dt * vel};
		Vector2f l2{dt * computeAcceleration(pos + 0.5f * k1, id)};
		Vector2f k2{dt * (vel + 0.5f * l1)};
		Vector2f l3{dt * computeAcceleration(pos + 0.5f * k2, id)};
		Vector2f k3{dt * (vel + 0.5f * l2)};
		Vector2f l4{dt * computeAcceleration(pos + k3, id)};
		Vector2f k4{dt * (vel + l3)};
		dx[id] = (k1 + 2.f * k2 + 2.f *  k3 + k4) / 6.f;
		dv[id] = (l1 + 2.f * l2 + 2.f *  l3 + l4) / 6.f;
	}

	for(EntityId id : _scene.view<Body>()) {
		Body& body{_scene.getComponent<Body>(id)};
		body.position += dx[id];
		body.velocity += dv[id];
		body.rotation += body.angularVelocity * dt;
        body.rotation = std::remainder(body.rotation, 2.f * pi);
	}
}
