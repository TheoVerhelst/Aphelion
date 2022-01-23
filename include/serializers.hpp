#ifndef SERIALIZERS_HPP
#define SERIALIZERS_HPP

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <json.hpp>

// Serializers for SFML classes
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

    template <typename T>
    struct adl_serializer<sf::Rect<T>> {
        static void to_json(json& j, const sf::Rect<T>& r) {
            j = {{"top", r.top}, {"left", r.left}, {"width", r.width}, {"height", r.height}};
        }

        static void from_json(const json& j, sf::Rect<T>& r) {
            j.at("top").get_to(r.top);
            j.at("left").get_to(r.left);
            j.at("width").get_to(r.width);
            j.at("height").get_to(r.height);
        }
    };

    template<>
    struct adl_serializer<sf::Time> {
        static void to_json(json& j, const sf::Time& time) {
            j = time.asSeconds();
        }

        static void from_json(const json& j, sf::Time& time) {
            time = sf::seconds(j.get<float>());
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

#endif // SERIALIZERS_HPP
