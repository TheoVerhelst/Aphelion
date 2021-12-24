#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <SFML/Graphics.hpp>
#include <PhysicalModel.hpp>

class Scene : public sf::Drawable {
public:
    Scene(const PhysicalModel& model);
	void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    const std::size_t _traceLength{1024};
    std::size_t _traceIndex{0};
    const PhysicalModel& _model;
	std::vector<sf::CircleShape> _circles;
    std::vector<sf::VertexArray> _traces;
};

#endif // SCENE_HPP
