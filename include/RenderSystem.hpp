#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <Scene.hpp>

class RenderSystem : public sf::Drawable {
public:
    RenderSystem(Scene& scene);
    void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    Scene& _scene;
};

#endif // RENDERSYSTEM_HPP
