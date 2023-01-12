#ifndef LIGHTSYSTEM_HPP
#define LIGHTSYSTEM_HPP

#include <array>
#include <vector>
#include <cstddef>
#include <SFML/Graphics/RenderTexture.hpp>
#include <vector.hpp>

// Forward declarations
namespace sf {
    class Shader;
    class RenderTarget;
    class ConvexShape;
}
class Scene;


class LightSystem {
public:
    LightSystem(Scene& scene, const sf::RenderTarget& renderTarget, sf::Shader& shader);
    void update();

private:
    Scene& _scene;
    const sf::RenderTarget& _renderTarget;
    sf::Shader& _shader;
    sf::RenderTexture _renderTexture;
    Vector2f _screenSize;

    std::vector<sf::ConvexShape> computeShadowShapes();
    std::vector<Vector2f> computeShadowGeometry(const std::vector<Vector2f>& shadowVertices,
            const Vector2f& lightSource, const std::array<Vector2f, 4>& view);
};

#endif // LIGHTSYSTEM_HPP
