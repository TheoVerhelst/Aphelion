#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <vector>
#include <utility>
#include <map>
#include <SFML/Graphics/Drawable.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <Animation.hpp>
#include <Action.hpp>
#include <vector.hpp>
#include <Scene.hpp>

enum class BodyType {
	Circle,
	Convex
};

struct Body {
	double mass;
	Vector2d position;
	Vector2d velocity;
	double rotation;
	double angularVelocity;
	double restitution;
	double friction;
	// The center of mass is used here only in the reference frame of the drawable shapes.
	// For example, for a sf::CircleShape, this is {radius, radius}.
	// The position vector is otherwise already pointing to the center of mass.
    Vector2d centerOfMass;
    double momentOfInertia;
	BodyType type;

	Vector2d localToWorld(const Vector2d& point) const;
	Vector2d worldToLocal(const Vector2d& point) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Body, mass, position, velocity, rotation, angularVelocity, restitution, friction)

struct CircleBody {
    double radius;

	Vector2d computeCenterOfMass() const;
	double computeMomentOfInertia(double mass) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleBody, radius)

struct ConvexBody {
    std::vector<Vector2d> vertices;

	Vector2d computeCenterOfMass() const;
	double computeMomentOfInertia(double mass, const Vector2d& centerOfMass) const;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConvexBody, vertices)


struct Collider {
	Vector2d supportFunction(const Vector2d& direction, const Scene& scene, EntityId id) const;
};

// The shadow function returns two points that indicate the start of the shadow
// cast by the body, located onto the body. Returns world coordinates.
struct Shadow {
	std::pair<Vector2d, Vector2d> shadowFunction(const Vector2d& lightSource, const Scene& scene, EntityId id) const;

private:
	std::pair<Vector2d, Vector2d> circleShadow(const Vector2d& lightSource, const Body& body, const CircleBody& circle) const;
	std::pair<Vector2d, Vector2d> convexShadow(const Vector2d& lightSource, const Body& body, const ConvexBody& convex) const;
};

// A light source is supposed to emit light from the COM of the body.
// This might change in the future. Shadows are computre with CircleBody and
// ConvexBody. Any colliding body casts shadows.
struct LightSource {
	double brightness;
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
