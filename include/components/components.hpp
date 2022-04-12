#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <vector.hpp>
#include <GameEvent.hpp>

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

typedef std::vector<GameEventType> ContinuousEvents;

struct ShipControl {
	bool playerRcsClockwise;
	bool playerRcsCounterClockwise;
	bool autoRcsClockwise;
	bool autoRcsCounterClockwise;
	float angularVelocityThreshold;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ShipControl, playerRcsClockwise,
	playerRcsCounterClockwise, autoRcsClockwise, autoRcsCounterClockwise,
	angularVelocityThreshold)


#endif // COMPONENTS_HPP
