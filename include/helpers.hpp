#ifndef HELPERS_HPP
#define HELPERS_HPP

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

// Returns a vector perpendicular to v, such that it has a positive dot product
// with the vector direction. It has the same norm as v.
template <FloatingPoint T>
sf::Vector2<T> perpendicular(const sf::Vector2<T>& v, const sf::Vector2<T>& direction) {
	Vector2d w{-v.y, v.x};
	// Reverse the direction in case we are in the wrong direction
	if(dot(w, direction) < 0) {
		return -w;
	}
	return w;
}

// Create stream operator for sf::Vector2
template <typename T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T> v) {
  return os << "(" << v.x << ", " << v.y << ")";
}

// Create serializers for sf::Vector2 and sf::Color
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

	template <>
    struct adl_serializer<sf::Color> {
        static void to_json(json& j, const sf::Color& c) {
			std::stringstream stream;
			stream << "#" << std::setfill('0') << std::setw(8) << std::hex << c.toInteger();
			j = {stream.str()};
        }

        static void from_json(const json& j, sf::Color& c) {
            sf::Uint32 value;
			std::stringstream stream(j.get<std::string>().substr(1));
			stream >> std::hex >> value;
			c = sf::Color(value);
        }
    };
}

#endif // HELPERS_HPP
