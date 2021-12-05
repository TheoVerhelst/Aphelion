#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <SFML/System/Vector2.hpp>

typedef sf::Vector2<double> Vector2d;
typedef sf::Vector2<float> Vector2f;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <FloatingPoint T>
T norm(const sf::Vector2<T>& vector) {
	return std::hypot(vector.x, vector.y);
}

template <FloatingPoint T>
T angle(const sf::Vector2<T>& vector) {
	return std::atan2(vector.y, vector.x);
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
