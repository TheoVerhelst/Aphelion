#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <string>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <SFML/Audio/Sound.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <vector.hpp>
#include <Event.hpp>

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

struct Player {
	struct Controls {
		bool rcsUp, rcsDown, rcsLeft, rcsRight, rcsClockwise, rcsCounterClockwise;
		bool engine;
	};
	Controls playerControls;
	Controls autoControls;
	float angularVelocityThreshold;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player::Controls, rcsUp, rcsDown, rcsLeft,
	rcsRight, rcsClockwise, rcsCounterClockwise, engine)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, playerControls, autoControls,
	angularVelocityThreshold)

enum class SoundEffectType {
	Collision
};
NLOHMANN_JSON_SERIALIZE_ENUM(SoundEffectType, {
	{SoundEffectType::Collision, "collision"}
})

struct SoundEffectData {
	std::string soundBuffer;
	sf::Sound sound;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SoundEffectData, soundBuffer)

typedef std::map<SoundEffectType, SoundEffectData> SoundEffects;

// We have to define the serialization manually, otherwise the SoundEffectType
// keys get converted to int, and the map is converted to an JSON array
void to_json(nlohmann::json& j, const SoundEffects& soundEffects);
void from_json(const nlohmann::json& j, SoundEffects& soundEffects);

#endif // COMPONENTS_HPP
