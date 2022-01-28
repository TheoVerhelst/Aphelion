#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Scene.hpp>

// Forward declarations
namespace sf {
    class Transformable;
    class Time;
}


class RenderSystem : public sf::Drawable {
public:
    RenderSystem(Scene& scene);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void update(sf::Time dt);

private:
    Scene& _scene;

    template <typename T>
    void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const {
        for (EntityId id : _scene.view<T>()) {
            target.draw(_scene.getComponent<T>(id), states);
        }
    }

    void updateTransformable(sf::Transformable& transformable, EntityId id) const;
};

#endif // RENDERSYSTEM_HPP
