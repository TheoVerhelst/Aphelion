#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <cstddef>
#include <functional>
#include <SFML/Graphics/VertexArray.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <vector.hpp>

struct Body {
	double mass;
	Vector2d position;
	Vector2d velocity;
	double rotation;
	double angularVelocity;
	double restitution;
	// The center of mass is used here only in relationship with drawable shapes.
	// The position vector is otherwise already pointing to the center of mass.
    Vector2d centerOfMass;
    double momentOfInertia;

	Vector2d localToWorld(const Vector2d& point) const;
	Vector2d worldToLocal(const Vector2d& point) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, mass, position, velocity, rotation, angularVelocity, restitution)

struct CircleBody {
    double radius;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, radius)

struct ConvexBody {
    std::vector<Vector2d> vertices;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConvexBody, vertices)

struct Trace {
    sf::VertexArray trace;
    static constexpr std::size_t traceLength{1024};
    std::size_t traceIndex{0};
};

struct Collider {
	std::function<Vector2d(const Vector2d&)> supportFunction;
};

#endif // COMPONENTS_HPP
