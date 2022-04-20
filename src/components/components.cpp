#include <components/components.hpp>

void to_json(nlohmann::json& j, const SoundEffects& soundEffects) {
    for (auto& [type, data] : soundEffects) {
		j[nlohmann::json(type).get<std::string>()] = data;
	}
}

void from_json(const nlohmann::json& j, SoundEffects& soundEffects) {
    for (auto& [typeValue, dataValue] : j.items()) {
		SoundEffectType type;
		nlohmann::json(typeValue).get_to(type);
		dataValue.get_to(soundEffects[type]);
	}
}
