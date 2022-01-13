#ifndef LIGHTSYSTEM_HPP
#define LIGHTSYSTEM_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <Scene.hpp>
#include <vector.hpp>

class LightSystem {
public:
    LightSystem(Scene& scene);
    void setRenderTarget(const sf::RenderTarget& target);
    void setShader(sf::Shader& shader);
    void update();

private:
    Scene& _scene;
    const sf::RenderTarget* _target;
    sf::Shader* _shader;
    sf::RenderTexture _renderTexture;
    // Limited because we cannot pass variable size array to the shader
    static constexpr std::size_t _maxLightSources{16};
    std::array<Vector2f, _maxLightSources> _lightPositions;
    std::array<float, _maxLightSources> _lightBrightnesses;
    int _numberLightSources;
    Vector2f _screenSize;
    float _distanceRatio;

    void updateLightSources();
    std::vector<sf::ConvexShape> computeShadowShapes();
    std::vector<Vector2d> computeShadowGeometry(const Vector2d& edgeA, const Vector2d& edgeB,
            const Vector2d& lightSource, const std::array<Vector2d, 4>& view);
};

#endif // LIGHTSYSTEM_HPP