#ifndef DEBUGINFO_HPP
#define DEBUGINFO_HPP

#include <SFML/Graphics.hpp>
#include <Body.hpp>
#include <vector.hpp>

class DebugInfo : public sf::Drawable {
public:
    DebugInfo();
    void update(const Body& body);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    class ArrowShape : public sf::Drawable {
    public:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        void setPoints(const Vector2f& begin, const Vector2f& end);

    private:
        sf::VertexArray _arrow{sf::Lines, 6};
        const float _headAngle{0.7};
        const float _headLength{5};
    };

    sf::CircleShape _position{3}; // Radius of the circle
    sf::Font _font;
    sf::Text _rotation{"", _font};
    sf::Text _angularVelocity{"", _font};
    ArrowShape _velocity; // Vector anchored at the center of mass
};

#endif // DEBUGINFO_HPP
