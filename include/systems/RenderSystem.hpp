#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <cstdint>
#include <SFML/Graphics/Drawable.hpp>
#include <Observer.hpp>
#include <Scene.hpp>

// Forward declarations
namespace sf {
    class Time;
    class Shader;
    class Transformable;
}
template <typename T>
class ResourceManager;


class RenderSystem : public sf::Drawable, public Observer<const sf::Time&> {
public:
    RenderSystem(Scene& scene, ResourceManager<sf::Shader>& shaderManager);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    virtual void update(const sf::Time& dt) override;

private:
    Scene& _scene;
    ResourceManager<sf::Shader>& _shaderManager;

    template <typename T>
    void drawComponent(sf::RenderTarget& target, sf::RenderStates states) const {
        for (EntityId id : _scene.view<T>()) {
            target.draw(_scene.getComponent<T>(id), states);
        }
    }

    void updateTransformable(sf::Transformable& transformable, EntityId id) const;
};

#endif // RENDERSYSTEM_HPP
