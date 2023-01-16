#include <json.hpp>
#include <fstream>
#include <iostream>
#include <cmath>
#include <BlackBodyTable.hpp>
#include <Paths.hpp>

BlackBodyTable::BlackBodyTable() {
    nlohmann::json j(nlohmann::json::parse(std::ifstream(Paths::getBlackBodyDataPath())));
    _table.resize(j["T"].size());
    _offset = j["T"][0];
    const float powerLow{std::log(j["M"][700 - _offset].get<float>())};
    const float powerHigh{std::log(j["M"][2500 - _offset].get<float>())};
    for (std::size_t i{0}; i < _table.size(); ++i) {
        _table[i].r = j["R"][i];
        _table[i].g = j["G"][i];
        _table[i].b = j["B"][i];
        const float power{j["M"][i]};
        // The alpha channel is a function of the log of the total spectrum power
        _table[i].a = static_cast<sf::Uint8>(255 * std::clamp(
            (std::log(power) - powerLow) / (powerHigh - powerLow),
            0.f, 1.f
        ));
    }
}
