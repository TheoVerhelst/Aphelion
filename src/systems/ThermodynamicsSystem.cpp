#include <cstddef>
#include <SFML/System/Time.hpp>
#include <systems/ThermodynamicsSystem.hpp>
#include <components/Body.hpp>
#include <components/Temperature.hpp>
#include <vector.hpp>
#include <Scene.hpp>
#include <GridField.hpp>
#include <PolarField.hpp>

ThermodynamicsSystem::ThermodynamicsSystem(Scene& scene):
    _scene{scene} {
}

void ThermodynamicsSystem::update(sf::Time timeStep) {
    float dt{timeStep.asSeconds()};

    for (auto& [id, body, temperature, polygonTemperature] :
            _scene.view<Body, Temperature, PolygonTemperature>()) {
        const GridField<float> field{polygonTemperature.field};
        Vector2s gridSize{field.getGridSize()};
        Vector2f cellSize{field.getCellSize()};

        for (std::size_t row{0}; row < gridSize.x; ++row) {
            for (std::size_t col{0}; col < gridSize.y; ++col) {
                const float T{field.at(row, col)};
                // Boundary conditions: imposing space derivative = 0 at the
                // boundaries can be implemented by taking T_{i+1}^t = T_{i-1}^t
                const float T_xm{field.at(row == 0 ? 1 : row - 1, col)};
                const float T_xp{field.at(row == gridSize.x - 1 ? row - 1 : row + 1, col)};
                const float T_ym{field.at(row, col == 0 ? 1 : col - 1)};
                const float T_yp{field.at(row, col == gridSize.y - 1 ? col - 1 : col + 1)};

                polygonTemperature.field.at(row, col) = T + dt * temperature.diffusivity * (
                    (T_xm + T_xp - 2 * T) / (cellSize.x * cellSize.x) +
                    (T_ym + T_yp - 2 * T) / (cellSize.y * cellSize.y)
                );
            }
        }
    }

    for (auto& [id, body, temperature, circleTemperature] :
            _scene.view<Body, Temperature, CircleTemperature>()) {
        const PolarField<float> field{circleTemperature.field};
        const float rhoStep{field.getRho(1)};
        const float thetaStep{field.getTheta(1)};
        const std::size_t rhoSteps{field.getRhoSteps()};
        const std::size_t thetaSteps{field.getThetaSteps()};

        float meanValue{0.f};
        for (std::size_t theta_i{0}; theta_i < thetaSteps; ++theta_i) {
            meanValue += field.at(1, theta_i);
        }
        meanValue /= static_cast<float>(thetaSteps);
        // No idea if this is the right way to solve the heat equation at the
        // pole, but it makes sense
        circleTemperature.field.at(0, 0) = field.at(0, 0) + 2 * dt * temperature.diffusivity * (
            (meanValue - field.at(0, 0)) / (rhoStep * rhoStep)
        );

        for (std::size_t theta_i{0}; theta_i < thetaSteps; ++theta_i) {
            for (std::size_t rho_i{1}; rho_i < rhoSteps; ++rho_i) {
                const float rho{field.getRho(rho_i)};
                const float T{field.at(rho_i, theta_i)};
                const float T_theta_m{field.at(rho_i, theta_i == 0 ? thetaSteps - 1 : theta_i - 1)};
                const float T_theta_p{field.at(rho_i, (theta_i + 1) % thetaSteps)};
                const float T_rho_m{field.at(rho_i - 1, theta_i)};
                // Boundary conditions
                const float T_rho_p{field.at(rho_i == rhoSteps - 1 ? rho_i - 1 : rho_i + 1, theta_i)};

                circleTemperature.field.at(rho_i, theta_i) = T + dt * temperature.diffusivity * (
                    (T_rho_m + T_rho_p - 2 * T) / (rhoStep * rhoStep) +
                    (T_rho_p - T_rho_m) / (2 * rhoStep * rho) +
                    (T_theta_m + T_theta_p - 2 * T) / (thetaStep * thetaStep * rho * rho)
                );
            }
        }
    }
}
