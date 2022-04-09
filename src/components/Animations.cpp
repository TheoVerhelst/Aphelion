#include <components/Animations.hpp>

void to_json(nlohmann::json& j, const Animations& animations) {
    for (auto& [action, animationData] : animations) {
		j[nlohmann::json(action).get<std::string>()] = animationData;
	}
}

void from_json(const nlohmann::json& j, Animations& animations) {
    for (auto& [actionValue, animationDataValue] : j.items()) {
		GameAction action;
		nlohmann::json(actionValue).get_to(action);
		animationDataValue.get_to(animations[action]);
	}
}
