#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <map>
#include <utility>
#include <cstdint>
#include <SFML/Graphics/Drawable.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <Animation.hpp>
#include <Action.hpp>
#include <vector.hpp>

// Forward declarations
class Scene;
typedef uint32_t EntityId;

enum class BodyType {
	Circle,
	Convex
};

struct Body {
	float mass;
	Vector2f position;
	Vector2f velocity;
	float rotation;
	float angularVelocity;
	float restitution;
	float friction;
	// The center of mass is used here only in the reference frame of the drawable shapes.
	// For example, for a sf::CircleShape, this is {radius, radius}.
	// The position vector is otherwise already pointing to the center of mass.
    Vector2f centerOfMass;
    float momentOfInertia;
	BodyType type;

	Vector2f localToWorld(const Vector2f& point) const;
	Vector2f worldToLocal(const Vector2f& point) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, mass, position, velocity, rotation, angularVelocity, restitution, friction)

struct CircleBody {
    float radius;

	Vector2f computeCenterOfMass() const;
	float computeMomentOfInertia(float mass) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, radius)

struct ConvexBody {
    std::vector<Vector2f> vertices;

	Vector2f computeCenterOfMass() const;
	float computeMomentOfInertia(float mass, const Vector2f& centerOfMass) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConvexBody, vertices)


struct Collider {
	Vector2f supportFunction(const Vector2f& direction, const Scene& scene, EntityId id) const;
};

// The shadow function returns two points that indicate the start of the shadow
// cast by the body, located onto the body. Returns world coordinates.
struct Shadow {
	std::pair<Vector2f, Vector2f> shadowFunction(const Vector2f& lightSource, const Scene& scene, EntityId id) const;

private:
	std::pair<Vector2f, Vector2f> circleShadow(const Vector2f& lightSource, const Body& body, const CircleBody& circle) const;
	std::pair<Vector2f, Vector2f> convexShadow(const Vector2f& lightSource, const Body& body, const ConvexBody& convex) const;
};

// A light source is supposed to emit light from the COM of the body.
// This might change in the future. Shadows are computre with CircleBody and
// ConvexBody. Any colliding body casts shadows.
struct LightSource {
	float brightness;
};

struct AnimationComponent : public sf::Drawable  {
	std::map<Action, Animation> animations;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

enum class MapElementType {
	CelestialBody,
	Ship
};
NLOHMANN_JSON_SERIALIZE_ENUM(MapElementType, {
    {MapElementType::CelestialBody, "celestialBody"},
    {MapElementType::Ship, "ship"},
})

struct MapElement {
	tgui::Picture::Ptr icon;
	MapElementType type;
};

// Tag component, it has no data but it used to find which entity is the player
struct Player {
};

#endif // COMPONENTS_HPP
