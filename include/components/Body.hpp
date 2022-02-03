#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <json.hpp>
#include <vector.hpp>
#include <serializers.hpp>

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

class ConvexComponent {
public:
	ConvexComponent(const std::vector<Vector2f>& vertices);
	float computeMomentOfInertia(float density, const Vector2f& axis) const;
    Vector2f supportFunction(const Vector2f& direction, const Body& body) const;
	float getArea() const;
	Vector2f getCenterOfMass() const;
	void updateCenterOfMass(const Vector2f& centerOfMass);

private:
	std::vector<Vector2f> _vertices;
	float _area;
	Vector2f _centerOfMass;

	void computeAreaAndCenterOfMass();
};

struct PolygonBody {
	std::vector<Vector2f> vertices;
    std::vector<ConvexComponent> components;

	PolygonBody() = default;
	PolygonBody(Body& body, const std::vector<Vector2f>& vertices);
	std::vector<Vector2f> shadowTerminator(const Vector2f& lightSource, const Body& body) const;

private:
	Vector2f computeCenterOfMass() const;
	float computeMomentOfInertia(float mass, const Vector2f& centerOfMass) const;
	static std::vector<std::vector<std::size_t>> earClipping(const std::vector<Vector2f>& vertices);
	static std::vector<std::vector<std::size_t>> HertelMehlhorn(
			const std::vector<Vector2f>& vertices,
			std::vector<std::vector<std::size_t>> triangulation);
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PolygonBody, vertices)

#endif // BODY_HPP
