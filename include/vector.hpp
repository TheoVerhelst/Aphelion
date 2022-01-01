#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <json.hpp>

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

template <FloatingPoint T>
T norm2(const sf::Vector2<T>& vector) {
	return dot(vector, vector);
}

// Vertical component of the cross product of the vector. We return a scalar
// instead of a Vector3 because we need only its norm.
template <FloatingPoint T>
T cross(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.y - a.y * b.x;
}

// Returns a vector perpendicular to v, such that it has a positive dot product
// with the vector d.
template <FloatingPoint T>
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

template <FloatingPoint T>
sf::Vector2<T> rotate(const sf::Vector2<T>& v, T angle) {
	return {v.x * std::cos(angle) - v.y * std::sin(angle), v.x * std::sin(angle) + v.y * std::cos(angle)};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> v) {
  return os << "(" << v.x << ", " << v.y << ")";
}

// Serializers for sf::Vector2
namespace nlohmann {
    template <typename T>
    struct adl_serializer<sf::Vector2<T>> {
        static void to_json(json& j, const sf::Vector2<T>& v) {
            j = {{"x", v.x}, {"y", v.y}};
        }

        static void from_json(const json& j, sf::Vector2<T>& v) {
            j.at("x").get_to(v.x);
            j.at("y").get_to(v.y);
        }
    };
}

#endif // VECTOR_HPP
