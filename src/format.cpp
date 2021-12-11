#include <iostream>
#include <iomanip>
#include <format.hpp>

std::string formatTime(double seconds) {
    std::stringstream ss;
    ss << std::setprecision(2) << seconds << " s";
    return ss.str();
}

std::string formatTime(const sf::Time& t) {
    return formatTime(t.asSeconds());
}
