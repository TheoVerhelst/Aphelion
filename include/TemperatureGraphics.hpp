#ifndef TEMPERATUREGRAPHICS_HPP
#define TEMPERATUREGRAPHICS_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

template <typename T>
class GridField;
template <typename T>
class PolarField;

class PolygonTemperatureGraphics : public sf::Drawable, public sf::Transformable {
public:
    void setupVertices(const GridField<float>& field);
    void update(const GridField<float>& field);

private:
    sf::VertexArray _vertices{sf::Triangles};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class CircleTemperatureGraphics : public sf::Drawable, public sf::Transformable {
public:
    void setupVertices(const PolarField<float>& field);
    void update(const PolarField<float>& field);

private:
    sf::VertexArray _vertices{sf::Triangles};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // TEMPERATUREGRAPHICS_HPP
