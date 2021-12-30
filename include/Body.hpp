#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <memory>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <vector.hpp>

using json = nlohmann::json;

class CircleBody;
class PolygonBody;

class Body {
public:
	Body(double mass, const Vector2d& position, const Vector2d& velocity,
		const sf::Color color, double angularVelocity);
	virtual ~Body() = default;
	virtual void collide(Body& other) = 0;
	virtual void collide(CircleBody& other) = 0;
	virtual void collide(PolygonBody& other) = 0;
	virtual std::shared_ptr<sf::Shape> createShape() const = 0;
	virtual Vector2d support(Vector2d direction) const = 0;
	double getMass() const;
	const Vector2d& getPosition() const;
	const Vector2d& getVelocity() const;
	const sf::Color& getColor() const;
	const double& getMomentOfInertia() const;
	const double& getAngularVelocity() const;
	void setMass(double mass);
	void setPosition(const Vector2d& position);
	void setVelocity(const Vector2d& velocity);
	void setColor(const sf::Color& color);
	void setAngularVelocity(const double& angularVelocity);

protected:
	double _mass;
	Vector2d _position;
	Vector2d _velocity;
	sf::Color _color;
	double _momentOfInertia;
	double _angularVelocity;

	// This must be called as soon as possible by subclasses
	void setMomentOfInertia(double momentOfInertia);
};


class CircleBody : public Body {
public:
	CircleBody(double mass, const Vector2d& position, const Vector2d& velocity,
		const sf::Color color, double angularVelocity, double radius);
	CircleBody(CircleBody&&) = default;
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;
	virtual Vector2d support(Vector2d direction) const override;
	double getRadius() const;

private:
	double _radius;

};


class PolygonBody : public Body {
public:
	PolygonBody(double mass, const Vector2d& position, const Vector2d& velocity,
		const sf::Color color, double angularVelocity, const std::vector<Vector2d>& vertices);
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;
	virtual Vector2d support(Vector2d direction) const override;
	const std::vector<Vector2d>& getVertices() const;

private:
	double computeMomentOfInertia() const;
	std::vector<Vector2d> _vertices;
};


namespace nlohmann {
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

    template <>
    struct adl_serializer<std::shared_ptr<CircleBody>> {
        static std::shared_ptr<CircleBody> from_json(const json& j) {
            return std::make_shared<CircleBody>(
				j.at("mass").get<double>(),
				j.at("position").get<Vector2d>(),
				j.at("velocity").get<Vector2d>(),
				j.at("color").get<sf::Color>(),
				j.at("angularVelocity").get<double>(),
				j.at("radius").get<double>()
			);
        }

        static void to_json(json& j, std::shared_ptr<CircleBody> body) {
			j = json{
				{"mass", body->getMass()},
				{"position", body->getPosition()},
				{"velocity", body->getVelocity()},
				{"color", body->getColor()},
				{"angularVelocity", body->getAngularVelocity()},
				{"radius", body->getRadius()}
			};
        }
    };

    template <>
    struct adl_serializer<std::shared_ptr<PolygonBody>> {
        static std::shared_ptr<PolygonBody> from_json(const json& j) {
            return std::make_shared<PolygonBody>(
				j.at("mass").get<double>(),
				j.at("position").get<Vector2d>(),
				j.at("velocity").get<Vector2d>(),
				j.at("color").get<sf::Color>(),
				j.at("angularVelocity").get<double>(),
				j.at("vertices").get<std::vector<Vector2d>>()
			);
        }

        static void to_json(json& j, std::shared_ptr<PolygonBody> body) {
			j = json{
				{"mass", body->getMass()},
				{"position", body->getPosition()},
				{"velocity", body->getVelocity()},
				{"color", body->getColor()},
				{"angularVelocity", body->getAngularVelocity()},
				{"vertices", body->getVertices()}
			};
        }
    };
}

#endif // BODY_HPP
