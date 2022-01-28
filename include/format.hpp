#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <string>

// Forward declarations
namespace sf {
    class Time;
}


std::string formatTime(float seconds);
std::string formatTime(sf::Time time);

#endif // FORMAT_HPP
