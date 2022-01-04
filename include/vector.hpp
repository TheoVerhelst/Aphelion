#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <iostream>
#include <SFML/System/Vector2.hpp>

typedef sf::Vector2<double> Vector2d;
typedef sf::Vector2<float> Vector2f;


template <typename T>
T dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.x + a.y * b.y;
}

template <std::floating_point T>
T norm(const sf::Vector2<T>& vector) {
	return std::hypot(vector.x, vector.y);
}

template <typename T>
T norm2(const sf::Vector2<T>& vector) {
	return dot(vector, vector);
}

// Vertical component of the cross product of the vector. We return a scalar
// instead of a Vector3 because we need only its norm.
template <typename T>
T cross(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.y - a.y * b.x;
}

// Returns a vector perpendicular to v, such that it has a positive dot product
// with the vector d.
template <typename T>
sf::Vector2<T> perpendicular(const sf::Vector2<T>& v, const sf::Vector2<T>& d) {
	// We could use the formula (v x d) x v, but this requires three
	// multiplications, which is not ideal for small vectors. We have a branch
	// but no number stability issue here.
	Vector2d res{v.y, -v.x};
	if (dot(res, d) < 0) {
		return -res;
	} else {
		return res;
	}
}

template <std::floating_point T>
sf::Vector2<T> rotate(const sf::Vector2<T>& v, T angle) {
	return {v.x * std::cos(angle) - v.y * std::sin(angle), v.x * std::sin(angle) + v.y * std::cos(angle)};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> v) {
  return os << "(" << v.x << ", " << v.y << ")";
}

#endif // VECTOR_HPP
