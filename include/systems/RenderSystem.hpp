#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <Scene.hpp>
#include <BlackBodyTable.hpp>

// Forward declarations
namespace sf {
    class Time;
}


class RenderSystem : public sf::Drawable {
public:
    RenderSystem(Scene& scene);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void update();

private:
    Scene& _scene;
    BlackBodyTable _table;
};

#endif // RENDERSYSTEM_HPP
