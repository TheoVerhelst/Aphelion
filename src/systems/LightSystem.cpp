#include <utility>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <systems/LightSystem.hpp>
#include <Scene.hpp>
#include <components/Body.hpp>
#include <components/components.hpp>

LightSystem::LightSystem(Scene& scene, const sf::RenderTarget& renderTarget, sf::Shader& shader):
    _scene{scene},
    _renderTarget{renderTarget},
    _shader{shader} {
    _renderTexture.create(1, 1);
}

void LightSystem::update() {
    const Vector2u uScreenSize(_renderTarget.getSize());
    _screenSize = static_cast<Vector2f>(uScreenSize);

    if (uScreenSize != _renderTexture.getSize()) {
        _renderTexture.create(uScreenSize.x, uScreenSize.y);
    }

    auto shadowShapes = computeShadowShapes();

    _renderTexture.clear(sf::Color::White);
    for (auto& shadowShape : shadowShapes) {
        shadowShape.setFillColor(sf::Color::Black);
        _renderTexture.draw(shadowShape);
    }
    _renderTexture.display();

    _shader.setUniform("texture", sf::Shader::CurrentTexture);
    _shader.setUniform("shadowTexture", _renderTexture.getTexture());
    _shader.setUniform("screenSize", _screenSize);
}

std::vector<sf::ConvexShape> LightSystem::computeShadowShapes() {
    int w(_renderTarget.getSize().x), h(_renderTarget.getSize().y);
    std::vector<Vector2i> corners{{0, 0}, {w, 0}, {w, h}, {0, h}};
    std::array<Vector2f, 4> viewArray;
    for (std::size_t i{0}; i < corners.size(); ++i) {
        viewArray[i] = _renderTarget.mapPixelToCoords(corners[i]);
    }

    // Fetch the list of shadow entities in advance to avoid repeating the call
    // in the loop
    auto shadowView = _scene.view<Body>();
    std::vector<sf::ConvexShape> shadowShapes;
    for (auto& [lightId, lightBody, _ignored_] : _scene.view<Body, LightSource>()) {
        const Vector2f lightSource{lightBody.position};

        for (auto& [shadowId, shadowBody] : shadowView) {
            if (lightId == shadowId) {
                continue;
            }
            // Compute the edges of the shadow
            const std::vector<Vector2f> shadowPoints{shadowBody.shadowTerminator(lightBody.position, _scene, shadowId)};
            // Compute the shadow geometry from the edges and the light source
            const auto shadowGeometry = computeShadowGeometry(shadowPoints, lightSource, viewArray);
            if (shadowGeometry.size() > 2) {
                // Convert to a sf::ConvexShape
                shadowShapes.emplace_back(shadowGeometry.size());
                for(std::size_t i{0}; i < shadowGeometry.size(); ++i) {
                    Vector2i targetCoord{_renderTarget.mapCoordsToPixel(shadowGeometry[i])};
                    shadowShapes.back().setPoint(i, static_cast<Vector2f>(targetCoord));
                }
            }
        }
    }
    return shadowShapes;
}

std::vector<Vector2f> LightSystem::computeShadowGeometry(const std::vector<Vector2f>& shadowVertices, const Vector2f& S, const std::array<Vector2f, 4>& view) {
    // Schematic representation, where S is the light source, and the box is the
    // occluding object. A and B are respectively the leftmost and the rightmost
    // vertices of the object when seen from the light source.
    //         A------+
    //         |      |
    //         |      |
    // S       |      |
    //         B------+

    const Vector2f& A{shadowVertices.front()};
    const Vector2f& B{shadowVertices.back()};

    // Iterate on every corner of the view
    std::vector<Vector2f> shadow;
    for (std::size_t i{0}; i < view.size(); ++i) {
        const Vector2f V1{view[i]};
        const Vector2f V2{view[(i + 1) % view.size()]};

        // Add the vertex V1 to the shape of the shadow if it is contained
        // between the two shadow edges. That is, if it is right of
        // (A - S) and left of (B - S). It also have to be further than the line
        // AB, so to the right of the vector A - B.
        const Vector2f normalA{perpendicular(A - S, false)};
        const Vector2f normalB{perpendicular(B - S, true)};
        const Vector2f normalBA{perpendicular(A - B, false)};
        if (dot(normalA, V1 - S) > 0 and dot(normalB, V1 - S) > 0
                and dot(normalBA, V1 - B) > 0) {
            shadow.push_back(V1);
        }
        // Add the intersections between the shadow edges and the view edge, and
        // between the line joining the shadow vertices and the view edge
        const auto [uA, vA] = intersection(V1, V2, S, A);
        const auto [uB, vB] = intersection(V1, V2, S, B);
        const auto [uC, vC] = intersection(V1, V2, B, A);
        std::array<std::tuple<float, float, std::string>, 3> intersections = {
            std::make_tuple(uA, vA, "A"),
            std::make_tuple(uB, vB, "B"),
            std::make_tuple(uC, vC, "C")
        };
        std::sort(intersections.begin(), intersections.end());
        for (auto& [u, v, name] : intersections) {
            if (0 < u and u < 1) {
                Vector2f P = V1 + u * (V2 - V1);
                if (name == "C" and 0 < v and v < 1) {
                    shadow.push_back(P);
                } else if (name != "C" and v > 1) {
                    if (name == "A" and boxContains(view, A)) {
                        shadow.push_back(A);
                    }
                    shadow.push_back(P);
                    if (name == "B" and boxContains(view, B)) {
                        shadow.push_back(B);
                    }
                }
            }
        }
    }
    return shadow;
}
