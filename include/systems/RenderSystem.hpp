#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <ResourceManager.hpp>
#include <Scene.hpp>

class RenderSystem : public sf::Drawable {
public:
    RenderSystem(Scene& scene, ResourceManager<sf::Shader>& shaderManager);
    void update(const sf::Time& dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

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
