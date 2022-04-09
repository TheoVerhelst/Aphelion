#include <components/Animations.hpp>

void to_json(nlohmann::json& j, const Animations& animations) {
    for (auto& [eventType, animationData] : animations) {
		j[nlohmann::json(eventType).get<std::string>()] = animationData;
	}
}

void from_json(const nlohmann::json& j, Animations& animations) {
    for (auto& [eventTypeValue, animationDataValue] : j.items()) {
		GameEventType type;
		nlohmann::json(eventTypeValue).get_to(type);
		animationDataValue.get_to(animations[type]);
	}
}
