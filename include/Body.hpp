#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <SFML/Graphics.hpp>
#include <helpers.hpp>
#include <json.hpp>

class CircleBody;
class PolygonBody;

class Body {
public:
	virtual ~Body() = default;
	virtual void collide(Body& other) = 0;
	virtual void collide(CircleBody& other) = 0;
	virtual void collide(PolygonBody& other) = 0;
	virtual std::shared_ptr<sf::Shape> createShape() const = 0;

	double mass;
	Vector2d position;
	Vector2d velocity;
	sf::Color color;

protected:
	virtual Vector2d support(Vector2d direction) const = 0;
};

class CircleBody : public Body {
public:
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;

	double radius;

protected:
	virtual Vector2d support(Vector2d direction) const override;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, mass, position, velocity, color, radius)

class PolygonBody : public Body {
public:
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;

	std::vector<Vector2d> vertices;

protected:
	virtual Vector2d support(Vector2d direction) const override;

private:
	static bool nearestSimplex(std::vector<Vector2d>& simplex, Vector2d& direction);
	static bool simplexLine(std::vector<Vector2d>& simplex, Vector2d& direction);
	static bool simplexTriangle(std::vector<Vector2d>& simplex, Vector2d& direction);
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PolygonBody, mass, position, velocity, color, vertices)

#endif // BODY_HPP
