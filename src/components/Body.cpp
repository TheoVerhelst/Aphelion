#include <algorithm>
#include <components/Body.hpp>
#include <Scene.hpp>

Vector2f Body::localToWorld(const Vector2f& point) const {
    return rotate(point, rotation) + position;
}

Vector2f Body::worldToLocal(const Vector2f& point) const {
    return rotate(point - position, -rotation);
}

std::vector<Vector2f> Body::shadowTerminator(const Vector2f& lightSource, const Scene& scene, EntityId id) const {
    if (scene.hasComponent<CircleBody>(id)) {
        return scene.getComponent<CircleBody>(id).shadowTerminator(lightSource, *this);
    } else {
        return scene.getComponent<PolygonBody>(id).shadowTerminator(lightSource, *this);
    }
}

CircleBody::CircleBody(Body& body, float radius_):
    radius{radius_} {
    body.mass = pi * radius * radius * body.density;
    body.centerOfMass = {radius, radius};
    body.momentOfInertia = body.mass * radius * radius / 2.f;
}

std::vector<Vector2f> CircleBody::shadowTerminator(const Vector2f& lightSource, const Body& body) const {
    // Get the left-sided normal vector
    Vector2f orthogonal{perpendicular(body.position - lightSource, true)};
    orthogonal /= norm(orthogonal);
    return {body.position + orthogonal * radius, body.position - orthogonal * radius};
}

PolygonBody::PolygonBody(Body& body, const std::vector<Vector2f>& vertices_):
    vertices{vertices_} {
    auto triangulation = earClipping(vertices);
	auto componentIndices = HertelMehlhorn(vertices, triangulation);
    for (std::vector<std::size_t>& indices : componentIndices) {
        std::vector<Vector2f> componentVertices;
        for (std::size_t index : indices) {
            componentVertices.push_back(vertices[index]);
        }
        components.emplace_back(componentVertices);
    }
    float area{0};
    body.centerOfMass = {0, 0};
    for (ConvexPolygon& component : components) {
        auto [compArea, compCenterOfMass] = component.areaAndCenterOfMass();
        area += compArea;
        body.centerOfMass += compArea * compCenterOfMass;
    }
    body.centerOfMass /= area;
    body.mass = area * body.density;
    body.momentOfInertia = 0;
    for (ConvexPolygon& component : components) {
        body.momentOfInertia += component.momentOfInertia(body.density, body.centerOfMass);
    }
}

std::vector<Vector2f> PolygonBody::shadowTerminator(const Vector2f& lightSource, const Body& body) const {
    std::vector<float> angles(vertices.size());
    std::vector<Vector2f> worldV;
    std::transform(vertices.begin(), vertices.end(), std::back_inserter(worldV),
        [&body] (const Vector2f& v) {
            return body.localToWorld(v - body.centerOfMass);
        }
    );
    angles[0] = 0.f;
    const float angle0{angle(worldV[0] - lightSource)};
    for (std::size_t i{1}; i < worldV.size(); ++i) {
        angles[i] = angle(worldV[i] - lightSource) - angle0;
    }
    // Find which vertices have the greatest and smallest angles. Greater angle
    // means the rightmost respective to the light source (since the y-axis goes
    // down).
    auto pair = std::minmax_element(angles.begin(), angles.end());
    const Vector2f A{*(worldV.begin() + std::distance(angles.begin(), pair.first))};
    const Vector2f B{*(worldV.begin() + std::distance(angles.begin(), pair.second))};
    // We make an approximation of the correct list of shadow vertices. Instead
    // of going along the shape, we just return two points outside of the shape
    // that result in the correct projected shadow, and such that the terminator
    // is still perpendicular to the light ray. First, we compute the vector
    // normal to the light ray.
    const Vector2f n{perpendicular(body.position - lightSource, true)};
    // Find the intersection between the normal vector and the light ray going
    // to A, and then to B
    auto [u, a] = intersection(body.position, body.position + n, lightSource, A);
    auto [v, b] = intersection(body.position, body.position + n, lightSource, B);
    return {body.position + n * u, body.position + n * v};
}

Vector2f PolygonBody::supportFunction(const Vector2f& direction, const ConvexPolygon& component, const Body& body) {
    return body.localToWorld(component.supportFunction(rotate(direction, -body.rotation)) - body.centerOfMass);
}
