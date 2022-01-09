#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <cstddef>
#include <functional>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <Animation.hpp>
#include <Action.hpp>
#include <vector.hpp>

struct Body {
	double mass;
	Vector2d position;
	Vector2d velocity;
	double rotation;
	double angularVelocity;
	double restitution;
	// The center of mass is used here only in the reference frame of the drawable shapes.
	// For example, for a sf::CircleShape, this is {radius, radius}.
	// The position vector is otherwise already pointing to the center of mass.
    Vector2d centerOfMass;
    double momentOfInertia;

	Vector2d localToWorld(const Vector2d& point) const;
	Vector2d worldToLocal(const Vector2d& point) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, mass, position, velocity, rotation, angularVelocity, restitution)

struct CircleBody {
    double radius;

	Vector2d computeCenterOfMass() const;
	double computeMomentOfInertia(double mass) const;
	Vector2d supportFunction(const Body& body, const Vector2d& direction) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, radius)

struct ConvexBody {
    std::vector<Vector2d> vertices;

	Vector2d computeCenterOfMass() const;
	double computeMomentOfInertia(double mass, const Vector2d& centerOfMass) const;
	Vector2d supportFunction(const Body& body, const Vector2d& direction) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConvexBody, vertices)

struct Trace : public sf::Drawable {
    sf::VertexArray trace;
    static constexpr std::size_t traceLength{1024};
    std::size_t traceIndex{0};

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

struct Collider {
	std::function<Vector2d(const Vector2d&)> supportFunction;
};

struct AnimationComponent : public sf::Drawable  {
	std::map<Action, Animation> animations;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

// Tag component, it has no data but it used to find which entity is the player
struct Player {
};

#endif // COMPONENTS_HPP
