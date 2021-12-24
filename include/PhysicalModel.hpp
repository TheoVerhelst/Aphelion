#ifndef PHYSICAL_MODEL_HPP
#define PHYSICAL_MODEL_HPP

#include <string>
#include <vector>
#include <memory>
#include <SFML/System/Time.hpp>
#include <Body.hpp>
#include <helpers.hpp>

class PhysicalModel {
public:
	PhysicalModel(const std::string& setupFile);
	void updateTime(const sf::Time& elapsedTime);
	void updateSteps(int steps);
	void setTimeScale(float timeScale);
	long long int getStepCounter() const;
	sf::Time getElapsedTime() const;
	std::vector<std::weak_ptr<const Body>> getBodies() const;

private:
	float _timeScale{1};
	sf::Time _timeStep{sf::seconds(0.02)};
	sf::Time _currentStep{sf::seconds(0)};
	// Might go back in time, so step counter is signed, and at least 64 bits.
	long long int _stepCounter{0};
	const double _gravitationalConstant{6.67408e-16};
	std::vector<std::shared_ptr<Body>> _bodies;

	/// Computes the acceleration field at a point given the other bodies.
	/// The body with given bodyIndex is ignored from the computation
	Vector2d computeAcceleration(const Vector2d position, std::size_t bodyIndex) const;
	void updateStep(bool backwards);
};

#endif // PHYSICAL_MODEL_HPP
