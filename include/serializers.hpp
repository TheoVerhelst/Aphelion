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
        static void to_json(json& j, sf::Time time) {
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

    // Define serialization for maps on enums because the default
    // behavior is to convert the enum to int and convert the result
    // to an array instead of a map.
    template <typename EnumType, typename T>
    struct adl_serializer<std::map<EnumType, T>> {
        static std::enable_if_t<std::is_enum_v<EnumType>>
                to_json(json& j, const std::map<EnumType, T>& map) {
            for (auto& [key, value] : map) {
		        j[nlohmann::json(key).get<std::string>()] = value;
	        }
        }

        static std::enable_if_t<std::is_enum_v<EnumType>>
                from_json(const json& j, std::map<EnumType, T>& map) {
            for (auto& [keyJson, valueJson] : j.items()) {
		        EnumType key;
		        nlohmann::json(keyJson).get_to(key);
		        valueJson.get_to(map[key]);
	        }
        }
    };
}

/*namespace sf {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VideoMode, width, height,
        bitsPerPixel)
    NLOHMANN_JSON_SERIALIZE_ENUM(Keyboard::Key, {
        {Keyboard::Unknown, "Unknown"},
        {Keyboard::A, "A"},
        {Keyboard::B, "B"},
        {Keyboard::C, "C"},
        {Keyboard::D, "D"},
        {Keyboard::E, "E"},
        {Keyboard::F, "F"},
        {Keyboard::G, "G"},
        {Keyboard::H, "H"},
        {Keyboard::I, "I"},
        {Keyboard::J, "J"},
        {Keyboard::K, "K"},
        {Keyboard::L, "L"},
        {Keyboard::M, "M"},
        {Keyboard::N, "N"},
        {Keyboard::O, "O"},
        {Keyboard::P, "P"},
        {Keyboard::Q, "Q"},
        {Keyboard::R, "R"},
        {Keyboard::S, "S"},
        {Keyboard::T, "T"},
        {Keyboard::U, "U"},
        {Keyboard::V, "V"},
        {Keyboard::W, "W"},
        {Keyboard::X, "X"},
        {Keyboard::Y, "Y"},
        {Keyboard::Z, "Z"},
        {Keyboard::Num0, "Num0"},
        {Keyboard::Num1, "Num1"},
        {Keyboard::Num2, "Num2"},
        {Keyboard::Num3, "Num3"},
        {Keyboard::Num4, "Num4"},
        {Keyboard::Num5, "Num5"},
        {Keyboard::Num6, "Num6"},
        {Keyboard::Num7, "Num7"},
        {Keyboard::Num8, "Num8"},
        {Keyboard::Num9, "Num9"},
        {Keyboard::Escape, "Escape"},
        {Keyboard::LControl, "LControl"},
        {Keyboard::LShift, "LShift"},
        {Keyboard::LAlt, "LAlt"},
        {Keyboard::LSystem, "LSystem"},
        {Keyboard::RControl, "RControl"},
        {Keyboard::RShift, "RShift"},
        {Keyboard::RAlt, "RAlt"},
        {Keyboard::RSystem, "RSystem"},
        {Keyboard::Menu, "Menu"},
        {Keyboard::LBracket, "LBracket"},
        {Keyboard::RBracket, "RBracket"},
        {Keyboard::Semicolon, "Semicolon"},
        {Keyboard::Comma, "Comma"},
        {Keyboard::Period, "Period"},
        {Keyboard::Quote, "Quote"},
        {Keyboard::Slash, "Slash"},
        {Keyboard::Backslash, "Backslash"},
        {Keyboard::Tilde, "Tilde"},
        {Keyboard::Equal, "Equal"},
        {Keyboard::Hyphen, "Hyphen"},
        {Keyboard::Space, "Space"},
        {Keyboard::Enter, "Enter"},
        {Keyboard::Backspace, "Backspace"},
        {Keyboard::Tab, "Tab"},
        {Keyboard::PageUp, "PageUp"},
        {Keyboard::PageDown, "PageDown"},
        {Keyboard::End, "End"},
        {Keyboard::Home, "Home"},
        {Keyboard::Insert, "Insert"},
        {Keyboard::Delete, "Delete"},
        {Keyboard::Add, "Add"},
        {Keyboard::Subtract, "Subtract"},
        {Keyboard::Multiply, "Multiply"},
        {Keyboard::Divide, "Divide"},
        {Keyboard::Left, "Left"},
        {Keyboard::Right, "Right"},
        {Keyboard::Up, "Up"},
        {Keyboard::Down, "Down"},
        {Keyboard::Numpad0, "Numpad0"},
        {Keyboard::Numpad1, "Numpad1"},
        {Keyboard::Numpad2, "Numpad2"},
        {Keyboard::Numpad3, "Numpad3"},
        {Keyboard::Numpad4, "Numpad4"},
        {Keyboard::Numpad5, "Numpad5"},
        {Keyboard::Numpad6, "Numpad6"},
        {Keyboard::Numpad7, "Numpad7"},
        {Keyboard::Numpad8, "Numpad8"},
        {Keyboard::Numpad9, "Numpad9"},
        {Keyboard::F1, "F1"},
        {Keyboard::F2, "F2"},
        {Keyboard::F3, "F3"},
        {Keyboard::F4, "F4"},
        {Keyboard::F5, "F5"},
        {Keyboard::F6, "F6"},
        {Keyboard::F7, "F7"},
        {Keyboard::F8, "F8"},
        {Keyboard::F9, "F9"},
        {Keyboard::F10, "F10"},
        {Keyboard::F11, "F11"},
        {Keyboard::F12, "F12"},
        {Keyboard::F13, "F13"},
        {Keyboard::F14, "F14"},
        {Keyboard::F15, "F15"},
        {Keyboard::Pause, "Pause"}
    })
}*/

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector) {
	os << "[";
	for (std::size_t i{0}; i < vector.size(); ++i) {
		os << vector[i] << (i < vector.size() - 1 ? ", " : "");
	}
	return os << "]";
}

#endif // SERIALIZERS_HPP
