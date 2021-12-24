#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <helpers.hpp>

class CircleBody;
class PolygonBody;

class Body {
public:
	virtual ~Body() = default;
	virtual void collide(Body& other) = 0;
	virtual void collide(CircleBody& other) = 0;
	virtual void collide(PolygonBody& other) = 0;

	double mass;
	Vector2d position;
	Vector2d velocity;
	sf::Color color;
};

class CircleBody : public Body {
public:
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	double radius;
};

class PolygonBody : public Body {
public:
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	std::vector<Vector2d> vertices;
};

#endif // BODY_HPP
