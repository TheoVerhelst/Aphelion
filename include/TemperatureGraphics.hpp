#ifndef TEMPERATUREGRAPHICS_HPP
#define TEMPERATUREGRAPHICS_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

template <typename T>
class GridField;
template <typename T>
class PolarField;
class BlackBodyTable;

sf::Color temperatureToColor(float temperature);

class PolygonTemperatureGraphics : public sf::Drawable, public sf::Transformable {
public:
    void setupVertices(const GridField<float>& field);
    void update(const GridField<float>& field, const BlackBodyTable& table);

private:
    sf::VertexArray _vertices{sf::Triangles};

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class CircleTemperatureGraphics : public sf::Drawable, public sf::Transformable {
public:
    CircleTemperatureGraphics();
    void setupVertices(const PolarField<float>& field);
    void update(const PolarField<float>& field, const BlackBodyTable& table);

private:
    sf::VertexArray _vertices{sf::Triangles};
    sf::Texture _whiteTexture;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // TEMPERATUREGRAPHICS_HPP
