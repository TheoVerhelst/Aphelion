#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <helpers.hpp>

struct Body {
	virtual ~Body() = default;
	virtual void collide(Body& other) = 0;

	double mass;
	Vector2d position;
	Vector2d velocity;
	sf::Color color;
};

struct CircleBody : public Body {
	virtual void collide(Body& other) override;

	double radius;
};

struct PolygonBody : public Body {
	virtual void collide(Body& other) override;

	std::vector<Vector2d> vertices;
};

#endif // BODY_HPP
