#include <json.hpp>
#include <fstream>
#include <BlackBodyTable.hpp>
#include <Paths.hpp>

BlackBodyTable::BlackBodyTable() {
    nlohmann::json j(nlohmann::json::parse(std::ifstream(Paths::getBlackBodyDataPath())));
    _table.resize(j["T"].size());
    _offset = j["T"][0];
    const float power_low{j["M"][700 - _offset]};
    const float power_high{j["M"][2500 - _offset]};
    for (std::size_t i{0}; i < _table.size(); ++i) {
        _table[i].r = j["R"][i];
        _table[i].g = j["G"][i];
        _table[i].b = j["B"][i];
        const float power{j["M"][i]};
        // The alpha channel is a linear function of the total spectrum power
        _table[i].a = static_cast<sf::Uint8>(255 * std::clamp(
            (power - power_low) / (power_high - power_low),
            0.f, 1.f
        ));
    }
}
