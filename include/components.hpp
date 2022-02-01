#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Rect.hpp>
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
	// The center of mass is in the reference frame of the drawable shapes. For
	// example, for a sf::CircleShape, this is {radius, radius}. The position
	// vector, on the other hand, is pointing to the center of mass.
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

struct Sprite {
	std::string texture;
	Vector2f offset;
	sf::IntRect rect;
	sf::Sprite sprite;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sprite, texture, offset, rect)

// A light source is supposed to emit light from the COM of the body.
// This might change in the future. Shadows are computre with CircleBody and
// ConvexBody. Any colliding body casts shadows.
struct LightSource {
	float brightness;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LightSource, brightness)

struct AnimationData {
	std::string texture;
	Vector2f offset;
	std::vector<AnimationFrame> frames;
	std::string soundBuffer;
	sf::Time soundLoopStart;
	sf::Time soundLoopEnd;
	Animation animation;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AnimationData, texture, offset, frames, soundBuffer, soundLoopStart, soundLoopEnd)

typedef std::map<Action, AnimationData> Animations;

// We have to define the serialization manually, otherwise the Action keys get
// converted to int, and the map is converted to an JSON array
void to_json(nlohmann::json& j, const Animations& animations);
void from_json(const nlohmann::json& j, Animations& animations);

enum class MapElementType {
	CelestialBody,
	Ship
};
NLOHMANN_JSON_SERIALIZE_ENUM(MapElementType, {
    {MapElementType::CelestialBody, "celestialBody"},
    {MapElementType::Ship, "ship"},
})

struct MapElement {
	MapElementType type;
	std::string tguiTexture;
	tgui::Picture::Ptr icon;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MapElement, type, tguiTexture)

// Tag component, it has no data but it used to find which entity is the player.
// The placeholder member is just used to make serialization easier.
struct Player {
	bool placeholder;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, placeholder)


struct Collider {
	Vector2f supportFunction(const Vector2f& direction, const Scene& scene, EntityId id) const;
};

// The shadow function returns two points that indicate the start of the shadow
// cast by the body, located onto the body. Returns world coordinates.
// TODO return a std::vector<Vector2f> to allow arbitrary shadow edges
struct Shadow {
	std::vector<Vector2f> shadowFunction(const Vector2f& lightSource, const Scene& scene, EntityId id) const;

private:
	std::vector<Vector2f> circleShadow(const Vector2f& lightSource, const Body& body, const CircleBody& circle) const;
	std::vector<Vector2f> convexShadow(const Vector2f& lightSource, const Body& body, const ConvexBody& convex) const;
};

#endif // COMPONENTS_HPP
