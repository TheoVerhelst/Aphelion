#include <iomanip>
#include <sstream>
#include <cmath>
#include <SFML/System/Time.hpp>
#include <format.hpp>

std::string formatTime(float seconds) {
    float sec{std::fmod(seconds, 60.f)};
    float min{std::trunc(seconds / 60.f)};
    float hour{std::trunc(seconds / 3600.f)};
    std::stringstream ss;
    if (hour > 0.f) {
        ss << hour << " h ";
    }
    if (min > 0.f) {
        ss << min << " min ";
    }
    ss << std::fixed << std::setprecision(2) << sec << " s";
    return ss.str();
}

std::string formatTime(const sf::Time& t) {
    return formatTime(t.asSeconds());
}
