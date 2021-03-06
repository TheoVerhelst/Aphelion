#ifndef PHYSICSSYSTEM_HPP
#define PHYSICSSYSTEM_HPP

#include <cstdint>
#include <SFML/System/Time.hpp>
#include <vector.hpp>

// Forward declarations
class Scene;
typedef std::uint32_t EntityId;


class PhysicsSystem {
public:
	PhysicsSystem(Scene& scene);
    void update(sf::Time dt);
	void updateSteps(int steps);
	void setTimeScale(float timeScale);
	long long int getStepCounter() const;
	sf::Time getElapsedTime() const;

private:
	Scene& _scene;
	float _timeScale{1};
	sf::Time _timeStep{sf::seconds(0.02f)};
	sf::Time _currentStep{sf::seconds(0)};
	// Might go back in time, so step counter is signed, and at least 64 bits.
	long long int _stepCounter{0};
	// Convert G from meters^3 kg/s^2 to px^3 kg/s^2
	const float _gravitationalConstant{6.67430e-11f * 36.f * 36.f * 36.f};

	/// Computes the acceleration field at a point given the other bodies.
	/// The body with given id is ignored from the computation
	Vector2f computeAcceleration(const Vector2f& position, EntityId id) const;
	void updateStep(bool backwards);
};

#endif // PHYSICSSYSTEM_HPP
