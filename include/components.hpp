#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <cstddef>
#include <functional>
#include <vector.hpp>
#include <SFML/Graphics/VertexArray.hpp>

struct Body {
	double mass;
	Vector2d position;
	Vector2d velocity;
	double rotation;
	double angularVelocity;
    //Vector2d centerOfMass;
    double momentOfInertia;

	Vector2d localToWorld(const Vector2d& point) const;
	Vector2d worldToLocal(const Vector2d& point) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, mass, position, velocity, rotation, angularVelocity)

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
