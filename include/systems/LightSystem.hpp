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
    // Limited because we cannot pass variable size array to the shader
    static constexpr std::size_t _maxLightSources{16};
    std::array<Vector2f, _maxLightSources> _lightPositions;
    std::array<float, _maxLightSources> _lightBrightnesses;
    int _numberLightSources;
    Vector2f _screenSize;

    void updateLightSources();
    std::vector<sf::ConvexShape> computeShadowShapes();
    std::vector<Vector2f> computeShadowGeometry(const std::vector<Vector2f>& shadowVertices,
            const Vector2f& lightSource, const std::array<Vector2f, 4>& view);
};

#endif // LIGHTSYSTEM_HPP
