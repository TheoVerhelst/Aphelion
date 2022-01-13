#include <algorithm>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <systems/LightSystem.hpp>
#include <components.hpp>

LightSystem::LightSystem(Scene& scene):
    _scene{scene} {
    _renderTexture.create(1, 1);
}

void LightSystem::setRenderTarget(const sf::RenderTarget& target) {
    _target = &target;
}

void LightSystem::setShader(sf::Shader& shader) {
    _shader = &shader;
}

void LightSystem::update() {
    const sf::View& view{_target->getView()};
    const Vector2u uScreenSize(_target->getViewport(view).width, _target->getViewport(view).height);
    _screenSize = static_cast<Vector2f>(uScreenSize);
    _distanceRatio = view.getSize().x / _screenSize.x;

    if (uScreenSize != _renderTexture.getSize()) {
        _renderTexture.create(uScreenSize.x, uScreenSize.y);
    }

    updateLightSources();
    auto shadowShapes = computeShadowShapes();

    _renderTexture.clear(sf::Color::White);
    for (auto& shadowShape : shadowShapes) {
        shadowShape.setFillColor(sf::Color::Black);
        _renderTexture.draw(shadowShape);
    }
    _renderTexture.display();

    _shader->setUniform("texture", sf::Shader::CurrentTexture);
    _shader->setUniform("shadowTexture", _renderTexture.getTexture());
    _shader->setUniformArray("lightPositions", _lightPositions.data(), _maxLightSources);
    _shader->setUniformArray("lightBrightnesses", _lightBrightnesses.data(), _maxLightSources);
    _shader->setUniform("numberLightSources", _numberLightSources);
    _shader->setUniform("viewMatrix", sf::Glsl::Mat3(view.getInverseTransform()));
    _shader->setUniform("screenSize", _screenSize);
}

void LightSystem::updateLightSources() {
    int i{0};
    for (EntityId lightId : _scene.view<Body, LightSource>()) {
        Body& body{_scene.getComponent<Body>(lightId)};
        LightSource& lightSource{_scene.getComponent<LightSource>(lightId)};
        _lightPositions[i] = static_cast<Vector2f>(body.position);
        _lightBrightnesses[i] = lightSource.brightness;
        ++i;
    }
    _numberLightSources = i;
}

std::vector<sf::ConvexShape> LightSystem::computeShadowShapes() {
    int w(_target->getSize().x), h(_target->getSize().y);
    std::vector<Vector2i> corners{{0, 0}, {w, 0}, {w, h}, {0, h}};
    std::array<Vector2d, 4> viewArray;
    for (std::size_t i{0}; i < corners.size(); ++i) {
        viewArray[i] = static_cast<Vector2d>(_target->mapPixelToCoords(corners[i]));
    }

    // Fetch the list of shadow entities in advance to avoid repeating the call
    // in the loop
    SceneView<Shadow> shadowScene{_scene.view<Shadow>()};
    std::vector<EntityId> shadowEntities;
    std::copy(shadowScene.begin(), shadowScene.end(), std::back_inserter(shadowEntities));

    std::vector<sf::ConvexShape> shadowShapes;
    for (EntityId lightId : _scene.view<Body, LightSource>()) {
        Body& body{_scene.getComponent<Body>(lightId)};
        const Vector2d lightSource{body.position};

        for (EntityId shadowId : shadowEntities) {
            if (lightId == shadowId) {
                continue;
            }
            const auto& shadowFunction = _scene.getComponent<Shadow>(shadowId).shadowFunction;
            // Compute the edges of the shadow
            const auto [A, B] = shadowFunction(body.position);
            // Compute the shadow geometry from the edges and the light source
            const auto shadowGeometry = computeShadowGeometry(A, B, lightSource, viewArray);
            if (shadowGeometry.size() > 2) {
                // Convert to a sf::ConvexShape
                shadowShapes.emplace_back(shadowGeometry.size());
                for(std::size_t i{0}; i < shadowGeometry.size(); ++i) {
                    Vector2i targetCoord{_target->mapCoordsToPixel(static_cast<Vector2f>(shadowGeometry[i]))};
                    shadowShapes.back().setPoint(i, static_cast<Vector2f>(targetCoord));
                }
            }
        }
    }
    return shadowShapes;
}

std::vector<Vector2d> LightSystem::computeShadowGeometry(const Vector2d& A,
    const Vector2d& B, const Vector2d& S, const std::array<Vector2d, 4>& view) {
    // Schematic representation, where S is the light source, and the box is the
    // occluding object. A and B are respectively the leftmost and the rightmost
    // vertices of the object when seen from the light source.
    //         A------+
    //         |      |
    //         |      |
    // S       |      |
    //         B------+

    // Check if A and B are in the view.
    bool containsA{boxContains(view, A)};
    bool containsB{boxContains(view, B)};

    // Compute intersection between the shadow edges and the view
    std::vector<Vector2d> shadow;
    for (std::size_t i{0}; i < view.size(); ++i) {
        const Vector2d V1{view[i]};
        const Vector2d V2{view[(i + 1) % view.size()]};

        // Add the vertex V1 to the shape of the shadow if it is contained
        // between the two shadow edges. That is, if it is right of
        // (A - S) and left of (B - S). It also have to be further than the line
        // AB, so to the right of the vector A - B.
        const Vector2d normalA{perpendicular(A - S, false)};
        const Vector2d normalB{perpendicular(B - S, true)};
        const Vector2d normalBA{perpendicular(A - B, false)};
        if (dot(normalA, V1 - S) > 0 and dot(normalB, V1 - S) > 0
                and dot(normalBA, V1 - B) > 0) {
            shadow.push_back(V1);
        }
        // Add the intersections between the shadow edges and the view edge, and
        // between the line joining the shadow vertices and the view edge
        const auto [uA, vA] = intersection(V1, V2, S, A);
        const auto [uB, vB] = intersection(V1, V2, S, B);
        const auto [uC, vC] = intersection(V1, V2, B, A);
        std::array<std::tuple<double, double, std::string>, 3> intersections = {
            std::make_tuple(uA, vA, "A"),
            std::make_tuple(uB, vB, "B"),
            std::make_tuple(uC, vC, "C")
        };
        std::sort(intersections.begin(), intersections.end());
        for (auto& [u, v, name] : intersections) {
            if (0 < u and u < 1) {
                Vector2d P = V1 + u * (V2 - V1);
                if (name == "C" and 0 < v and v < 1) {
                    shadow.push_back(P);
                } else if (name != "C" and v > 1) {
                    if (name == "A" and containsA) {
                        shadow.push_back(A);
                    }
                    shadow.push_back(P);
                    if (name == "B" and containsB) {
                        shadow.push_back(B);
                    }
                }
            }
        }
    }
    return shadow;
}
