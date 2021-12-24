#include <Scene.hpp>
#include <Body.hpp>
#include <memory>

Scene::Scene(const PhysicalModel& model):
        _model{model} {

    auto bodies = _model.getBodies();
	for (std::size_t i{0}; i < bodies.size(); ++i) {
        auto body = dynamic_pointer_cast<const CircleBody>(bodies[i].lock());
    	sf::CircleShape circle{static_cast<float>(body->radius)};
        circle.setOrigin(body->radius, body->radius);
    	circle.setFillColor(body->color);
    	_circles.push_back(circle);

        // Fill the trace with points at the position of the body, to avoid
        // undefined traces because of the unitialized vector.
        sf::VertexArray trace(sf::Lines, _traceLength * 2);
        for (std::size_t j{0}; j < _traceLength * 2; ++j) {
            trace[j] = sf::Vertex(static_cast<Vector2f>(body->position), body->color);
        }
        _traces.push_back(trace);
	}
}

void Scene::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for(auto& trace : _traces) {
        target.draw(trace, states);
    }

	for (auto& circle : _circles) {
		target.draw(circle, states);
	}
}

void Scene::update() {
    auto bodies = _model.getBodies();
    std::size_t nextIndex{(_traceIndex + 1) % _traceLength};

	for (std::size_t i{0}; i < bodies.size(); ++i) {
        auto body = dynamic_pointer_cast<const CircleBody>(bodies[i].lock());
		_circles[i].setPosition(static_cast<Vector2f>(body->position));

        // The new trace line is from the previous one to the new position
        _traces[i][nextIndex * 2] = _traces[i][(_traceIndex * 2) + 1];
        _traces[i][nextIndex * 2 + 1] = sf::Vertex(
            static_cast<sf::Vector2f>(body->position),
            body->color
        );
        // Adjust the trace transparency to make it fade away
        for(std::size_t j{0}; j < _traceLength; ++j) {
            float alpha{255.f * static_cast<float>(j) / static_cast<float>(_traceLength)};
            std::size_t index{((nextIndex + j) % _traceLength) * 2};
            _traces[i][index].color.a = static_cast<sf::Uint8>(alpha);
            _traces[i][index + 1].color.a = static_cast<sf::Uint8>(alpha);
        }

	}
    _traceIndex = nextIndex;
}
