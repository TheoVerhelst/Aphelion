#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <SFML/System/Vector2.hpp>

typedef sf::Vector2<double> Vector2d;

template <typename T>
T norm(const sf::Vector2<T>& vector) {
	return std::hypot(vector.x, vector.y);
}

template <typename T>
T wrap(T value, T max) {
	while (value > max)
		value -= max;
	while (value < static_cast<T>(0))
		value += max;
	return value;
}

#endif // HELPERS_HPP
