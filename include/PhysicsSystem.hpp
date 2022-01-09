#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <cstddef>
#include <SFML/System/Time.hpp>
#include <Scene.hpp>
#include <CollisionSystem.hpp>
#include <components.hpp>
#include <vector.hpp>

class PhysicsSystem {
public:
	PhysicsSystem(Scene& scene);
	void updateTime(const sf::Time& elapsedTime);
	void updateSteps(int steps);
	void setTimeScale(float timeScale);
	long long int getStepCounter() const;
	sf::Time getElapsedTime() const;

private:
	Scene& _scene;
	CollisionSystem _collisionSystem;
	float _timeScale{1};
	sf::Time _timeStep{sf::seconds(0.02)};
	sf::Time _currentStep{sf::seconds(0)};
	// Might go back in time, so step counter is signed, and at least 64 bits.
	long long int _stepCounter{0};
	const double _gravitationalConstant{6.67430e-11};

	/// Computes the acceleration field at a point given the other bodies.
	/// The body with given id is ignored from the computation
	Vector2d computeAcceleration(const Vector2d& position, EntityId id) const;
	void updateStep(bool backwards);
};

#endif // PHYSICSSYSTEM_HPP
