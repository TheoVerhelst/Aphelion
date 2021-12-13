#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <iostream>
#include <SFML/System/Vector2.hpp>

typedef sf::Vector2<double> Vector2d;
typedef sf::Vector2<float> Vector2f;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <FloatingPoint T>
T dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.x + a.y * b.y;
}

template <FloatingPoint T>
T norm(const sf::Vector2<T>& vector) {
	return std::hypot(vector.x, vector.y);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> v) {
  return os << "(" << v.x << ", " << v.y << ")";
}


#endif // HELPERS_HPP
