#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <json.hpp>
#include <vector.hpp>
#include <serializers.hpp>
#include <polygon.hpp>
#include <GridField.hpp>
#include <PolarField.hpp>

class Scene;
typedef uint32_t EntityId;

struct Body {
	float density;
	Vector2f position;
	Vector2f velocity;
	float rotation;
	float angularVelocity;
	float restitution;
	float friction;
	// The center of mass is in the reference frame of the drawable shapes. For
	// example, for a sf::CircleShape, this is {radius, radius}. The position
	// vector, on the other hand, is pointing to the center of mass.
    Vector2f centerOfMass;
    float momentOfInertia;
	float mass;

	Vector2f localToWorld(const Vector2f& point) const;
	Vector2f worldToLocal(const Vector2f& point) const;
    std::vector<Vector2f> shadowTerminator(const Vector2f& lightSource, const Scene& scene, EntityId id) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, density, position, velocity, rotation, angularVelocity, restitution, friction)

struct CircleBody {
	float radius;

	CircleBody() = default;
	CircleBody(Body& body, float radius);
	std::vector<Vector2f> shadowTerminator(const Vector2f& lightSource, const Body& body) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, radius)

struct PolygonBody {
	std::vector<Vector2f> vertices;
    std::vector<ConvexPolygon> components;

	PolygonBody() = default;
	PolygonBody(Body& body, const std::vector<Vector2f>& vertices);
	std::vector<Vector2f> shadowTerminator(const Vector2f& lightSource, const Body& body) const;
	static Vector2f supportFunction(const Vector2f& direction, const ConvexPolygon& component, const Body& body);
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PolygonBody, vertices)

struct Temperature {
    float conductivity;
    float specificCapacity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Temperature, conductivity, specificCapacity)

struct CircleTemperature {
    PolarField<float> field;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleTemperature, field)

struct PolygonTemperature {
    GridField<float> field;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PolygonTemperature, field)

#endif // BODY_HPP
