#include <cstddef>
#include <SFML/Graphics/RenderTarget.hpp>
#include <TemperatureGraphics.hpp>
#include <components/Temperature.hpp>
#include <GridField.hpp>
#include <PolarField.hpp>

void PolygonTemperatureGraphics::setupVertices(const GridField<float>& field) {
    const std::size_t rows{field.getGridSize().x};
    const std::size_t cols{field.getGridSize().y};
    _vertices.resize((rows - 1) * (cols - 1) * 6);

    for (std::size_t i{0}; i < rows - 1; ++i) {
        for (std::size_t j{0}; j < cols - 1; ++j) {
            const std::size_t idx{(i * cols + j) * 6};
            _vertices[idx + 0].position = field.getPos(i, j);
            _vertices[idx + 1].position = field.getPos(i + 1, j);
            _vertices[idx + 2].position = field.getPos(i + 1, j + 1);
            _vertices[idx + 3].position = field.getPos(i, j);
            _vertices[idx + 4].position = field.getPos(i + 1, j + 1);
            _vertices[idx + 5].position = field.getPos(i, j + 1);
        }
    }
}

void PolygonTemperatureGraphics::update(const GridField<float>& field) {
    const std::size_t cols{field.getGridSize().y};
    for (std::size_t i{0}; i < field.getGridSize().x - 1; ++i) {
        for (std::size_t j{0}; j < cols - 1; ++j) {
            const std::size_t idx{(i * cols + j) * 6};
            _vertices[idx + 0].color = sf::Color::Red;
            _vertices[idx + 1].color = sf::Color::Red;
            _vertices[idx + 2].color = sf::Color::Red;
            _vertices[idx + 3].color = sf::Color::Red;
            _vertices[idx + 4].color = sf::Color::Red;
            _vertices[idx + 5].color = sf::Color::Red;
        }
    }
}

void PolygonTemperatureGraphics::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(_vertices, states);
}

CircleTemperatureGraphics::CircleTemperatureGraphics() {
    _whiteTexture.create(1, 1);
    sf::Image image;
    image.create(1, 1, sf::Color::White);
    _whiteTexture.update(image);
}

void CircleTemperatureGraphics::setupVertices(const PolarField<float>& field) {
    const std::size_t rhoSteps{field.getRhoSteps()};
    const std::size_t thetaSteps{field.getThetaSteps()};
    const std::size_t verticesPerSection{(rhoSteps - 1) * 6 - 3};
    _vertices.resize(thetaSteps * verticesPerSection);

    for (std::size_t theta_i{0}; theta_i < thetaSteps; ++theta_i) {
        const std::size_t theta_ip{(theta_i + 1) % thetaSteps};
        const std::size_t idx{theta_i * verticesPerSection};
        _vertices[idx + 0].position = Vector2f(0, 0);
        _vertices[idx + 1].position = field.getCartesian(1, theta_i);
        _vertices[idx + 2].position = field.getCartesian(1, theta_ip);

        for (std::size_t rho_i{1}; rho_i < rhoSteps - 1; ++rho_i) {
            const std::size_t rho_ip{rho_i + 1};
            const std::size_t jdx{idx + 3 + (rho_i - 1) * 6};
            _vertices[jdx + 0].position = field.getCartesian(rho_i,  theta_i);
            _vertices[jdx + 1].position = field.getCartesian(rho_ip, theta_i);
            _vertices[jdx + 2].position = field.getCartesian(rho_ip, theta_ip);
            _vertices[jdx + 3].position = field.getCartesian(rho_i,  theta_i);
            _vertices[jdx + 4].position = field.getCartesian(rho_ip, theta_ip);
            _vertices[jdx + 5].position = field.getCartesian(rho_i,  theta_ip);
        }
    }
}

void CircleTemperatureGraphics::update(const PolarField<float>& field) {
    const std::size_t rhoSteps{field.getRhoSteps()};
    const std::size_t thetaSteps{field.getThetaSteps()};
    const std::size_t verticesPerSection{(rhoSteps - 1) * 6 - 3};

    for (std::size_t theta_i{0}; theta_i < thetaSteps; ++theta_i) {
        const std::size_t idx{theta_i * verticesPerSection};
        _vertices[idx + 0].color = sf::Color::Red;
        _vertices[idx + 1].color = sf::Color::Red;
        _vertices[idx + 2].color = sf::Color::Red;

        for (std::size_t rho_i{1}; rho_i < rhoSteps - 1; ++rho_i) {
            const std::size_t jdx{idx + 3 + (rho_i - 1) * 6};
            _vertices[jdx + 0].color = sf::Color::Red;
            _vertices[jdx + 1].color = sf::Color::Red;
            _vertices[jdx + 2].color = sf::Color::Red;
            _vertices[jdx + 3].color = sf::Color::Red;
            _vertices[jdx + 4].color = sf::Color::Red;
            _vertices[jdx + 5].color = sf::Color::Red;
        }
    }
}

void CircleTemperatureGraphics::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &_whiteTexture;
    target.draw(_vertices, states);
}
