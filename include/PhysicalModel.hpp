#ifndef PHYSICAL_MODEL_HPP
#define PHYSICAL_MODEL_HPP

#include <SFML/Graphics.hpp>
#include <Body.hpp>
#include <helpers.hpp>

class PhysicalModel : public sf::Drawable {
public:
	PhysicalModel(const std::string& setupFile);
	void updateTime(const sf::Time& elapsedTime);
	void updateSteps(int steps);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setCanvasSize(Vector2f size);
	void setPixelsByMeter(float pixelsByMeter);
	void setTimeScale(float timeScale);
	long long int getStepCounter() const;
	sf::Time getElapsedTime() const;

private:
	float _pixelsByMeter{1};
	float _timeScale{1};
	sf::Time _timeStep{sf::seconds(0.02)};
	sf::Time _currentStep{sf::seconds(0)};
	// Might go back in time, so step counter is signed, and at least 64 bits.
	long long int _stepCounter{0};
	const double _gravitationalConstant{6.67408e-16};
	std::vector<Body> _bodies;
	std::vector<sf::CircleShape> _circles;

	/// Computes the acceleration field at a point given the other bodies.
	/// The body with given bodyIndex is ignored from the computation
	Vector2d computeAcceleration(const Vector2d position, std::size_t bodyIndex) const;
	void updateStep(bool backwards);
	void updateGraphics();
};

#endif // PHYSICAL_MODEL_HPP
