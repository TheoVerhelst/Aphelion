#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#include <string>
#include <vector>
#include <map>
#include <SFML/System/Time.hpp>
#include <json.hpp>
#include <serializers.hpp>
#include <Animation.hpp>
#include <vector.hpp>

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

enum class AnimationType {
	Engine,
	RcsUp,
	RcsDown,
	RcsLeft,
	RcsRight,
	RcsClockwise,
	RcsCounterClockwise
};
NLOHMANN_JSON_SERIALIZE_ENUM(AnimationType, {
	{AnimationType::Engine, "engine"},
	{AnimationType::RcsUp, "rcsUp"},
	{AnimationType::RcsDown, "rcsDown"},
	{AnimationType::RcsLeft, "rcsLeft"},
	{AnimationType::RcsRight, "rcsRight"},
	{AnimationType::RcsClockwise, "rcsClockwise"},
	{AnimationType::RcsCounterClockwise, "rcsCounterClockwise"}
})

typedef std::map<AnimationType, AnimationData> Animations;

// We have to define the serialization manually, otherwise the Action keys get
// converted to int, and the map is converted to an JSON array
void to_json(nlohmann::json& j, const Animations& animations);
void from_json(const nlohmann::json& j, Animations& animations);

#endif // ANIMATIONS_HPP
