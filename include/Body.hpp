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
		double rotation, double angularVelocity, const sf::Color color);
	virtual ~Body() = default;
	virtual void collide(Body& other) = 0;
	virtual void collide(CircleBody& other) = 0;
	virtual void collide(PolygonBody& other) = 0;
	virtual std::shared_ptr<sf::Shape> createShape() const = 0;
	// In world coordinates
	virtual Vector2d support(const Vector2d& direction) const = 0;
	double getMass() const;
	// In world coordinates
	const Vector2d& getPosition() const;
	const Vector2d& getVelocity() const;
	double getRotation() const;
	double getAngularVelocity() const;
	// In local coordinates
	virtual Vector2d getCenterOfMass() const = 0;
	virtual double getMomentOfInertia() const = 0;
	const sf::Color& getColor() const;
	void setMass(double mass);
	// In world coordinates
	void setPosition(const Vector2d& position);
	void setVelocity(const Vector2d& velocity);
	void setRotation(double rotation);
	void setAngularVelocity(double angularVelocity);
	void setColor(const sf::Color& color);

protected:
	double _mass;
	Vector2d _position;
	Vector2d _velocity;
	double _rotation;
	double _angularVelocity;
	sf::Color _color;
};


class CircleBody : public Body {
public:
	CircleBody(double mass, const Vector2d& position, const Vector2d& velocity,
		double rotation, double angularVelocity, const sf::Color color, double radius);
	CircleBody(CircleBody&&) = default;
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;
	virtual Vector2d support(const Vector2d& direction) const override;
	virtual Vector2d getCenterOfMass() const override;
	virtual double getMomentOfInertia() const override;
	double getRadius() const;

private:
	double _radius;

};


class PolygonBody : public Body {
public:
	PolygonBody(double mass, const Vector2d& position, const Vector2d& velocity,
		double rotation, double angularVelocity, const sf::Color color,
		const std::vector<Vector2d>& vertices);
	virtual void collide(Body& other) override;
	virtual void collide(CircleBody& other) override;
	virtual void collide(PolygonBody& other) override;
	virtual std::shared_ptr<sf::Shape> createShape() const override;
	virtual Vector2d support(const Vector2d& direction) const override;
	virtual Vector2d getCenterOfMass() const override;
	virtual double getMomentOfInertia() const override;
	// In local coordinates
	const std::vector<Vector2d>& getVertices() const;

private:
	double computeMomentOfInertia() const;
	Vector2d computeCenterOfMass() const;

	std::vector<Vector2d> _vertices;
	Vector2d _centerOfMass;
	double _momentOfInertia;
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
				j.at("rotation").get<double>(),
				j.at("angularVelocity").get<double>(),
				j.at("color").get<sf::Color>(),
				j.at("radius").get<double>()
			);
        }

        static void to_json(json& j, std::shared_ptr<CircleBody> body) {
			j = json{
				{"mass", body->getMass()},
				{"position", body->getPosition()},
				{"velocity", body->getVelocity()},
				{"rotation", body->getRotation()},
				{"angularVelocity", body->getAngularVelocity()},
				{"color", body->getColor()},
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
				j.at("rotation").get<double>(),
				j.at("angularVelocity").get<double>(),
				j.at("color").get<sf::Color>(),
				j.at("vertices").get<std::vector<Vector2d>>()
			);
        }

        static void to_json(json& j, std::shared_ptr<PolygonBody> body) {
			j = json{
				{"mass", body->getMass()},
				{"position", body->getPosition()},
				{"velocity", body->getVelocity()},
				{"rotation", body->getRotation()},
				{"angularVelocity", body->getAngularVelocity()},
				{"color", body->getColor()},
				{"vertices", body->getVertices()}
			};
        }
    };
}

#endif // BODY_HPP
