#ifndef PHYSICAL_MODEL_HPP
#define PHYSICAL_MODEL_HPP

#include <SFML/Graphics.hpp>
#include <Body.hpp>
#include <helpers.hpp>

class PhysicalModel : public sf::Drawable {
public:
	PhysicalModel(const std::string& filename);
	void update(const sf::Time& elapsedTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setPixelsByMeter(float pixelsByMeter);
	void setTimeScale(double timeScale);

private:
	float _pixelsByMeter{1};
	double _timeScale{1};
	const double _gravitationalConstant{6.67408e-16};
	Vector2d _limitOrigin{10., 10.};
	Vector2d _limitSize{800., 800.};
	sf::RectangleShape _limitRectangle{
		{static_cast<float>(_limitSize.x), static_cast<float>(_limitSize.y)}
	};
	std::vector<Body> _bodies;
	std::vector<sf::CircleShape> _circles;

	/// Computes the acceleration field at a point given the other bodies.
	/// The body with given bodyIndex is ignored from the computation
	Vector2d computeAcceleration(const Vector2d position, std::size_t bodyIndex) const;
};

#endif // PHYSICAL_MODEL_HPP
