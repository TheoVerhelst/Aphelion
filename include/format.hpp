#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <string>
#include <SFML/System/Time.hpp>

std::string formatTime(double seconds);
std::string formatTime(const sf::Time& time);

#endif // FORMAT_HPP
