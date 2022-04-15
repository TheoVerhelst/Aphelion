#include <components/Animations.hpp>

void to_json(nlohmann::json& j, const Animations& animations) {
    for (auto& [type, data] : animations) {
		j[nlohmann::json(type).get<std::string>()] = data;
	}
}

void from_json(const nlohmann::json& j, Animations& animations) {
    for (auto& [typeValue, dataValue] : j.items()) {
		AnimationType type;
		nlohmann::json(typeValue).get_to(type);
		dataValue.get_to(animations[type]);
	}
}
