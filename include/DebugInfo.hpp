#ifndef DEBUGINFO_HPP
#define DEBUGINFO_HPP

#include <SFML/Graphics.hpp>
#include <components.hpp>
#include <vector.hpp>

class DebugInfo : public sf::Drawable {
public:
    DebugInfo(const sf::Font& font);
    void update(const Body& body);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    class ArrowShape : public sf::Drawable {
    public:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void setPoints(const Vector2f& begin, const Vector2f& end);
        void setColor(const sf::Color& color);

    private:
        sf::VertexArray _arrow{sf::Lines, 6};
        float _headAngle{0.7};
        float _headLength{5};
    };

    const sf::Font& _font;
    sf::CircleShape _position{3}; // Radius of the circle
    sf::Text _rotation;
    sf::Text _angularVelocity;
    ArrowShape _velocity; // Vector anchored at the center of mass
};

#endif // DEBUGINFO_HPP
