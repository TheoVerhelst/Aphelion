#include <format.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

std::string formatTime(double seconds) {
    double sec{std::fmod(seconds, 60)};
    double min{std::trunc(seconds / 60)};
    double hour{std::trunc(seconds / 3600)};
    std::stringstream ss;
    if (hour > 0) {
        ss << hour << " h ";
    }
    if (min > 0) {
        ss << min << " min ";
    }
    ss << std::fixed << std::setprecision(2) << sec << " s";
    return ss.str();
}

std::string formatTime(const sf::Time& t) {
    return formatTime(t.asSeconds());
}
