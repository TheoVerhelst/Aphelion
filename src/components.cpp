#include <algorithm>
#include <SFML/Graphics/RenderTarget.hpp>
#include <components.hpp>
#include <Scene.hpp>

Vector2f Body::localToWorld(const Vector2f& point) const {
    return rotate(point, rotation) + position;
}

Vector2f Body::worldToLocal(const Vector2f& point) const {
    return rotate(point - position, -rotation);
}

Vector2f CircleBody::computeCenterOfMass() const {
    return {radius, radius};
}

float CircleBody::computeMomentOfInertia(float mass) const {
    return mass * radius * radius / 2.f;
}

Vector2f ConvexBody::computeCenterOfMass() const {
	if (vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
	std::vector<float> triangleAreas;
	std::vector<Vector2f> triangleCenters;
    const Vector2f A{vertices[0]};
	for (std::size_t i{1}; i < vertices.size() - 1; ++i) {
		Vector2f B{vertices[i]};
		Vector2f C{vertices[i + 1]};
		triangleAreas.push_back(std::abs(cross(B - A, C - A)) / 2.f);
		triangleCenters.push_back((A + B + C) / 3.f);
	}
	float totalArea{std::accumulate(triangleAreas.begin(), triangleAreas.end(), 0.f)};
	Vector2f centerOfMass{std::inner_product(triangleCenters.begin(),
		triangleCenters.end(), triangleAreas.begin(), Vector2f(0.f, 0.f))};
	return centerOfMass / totalArea;
}

float ConvexBody::computeMomentOfInertia(float mass, const Vector2f& centerOfMass) const {
	if (vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
    std::vector<float> areas;
    for (std::size_t i{0}; i < vertices.size(); ++i) {
        Vector2f B{vertices[i]};
        Vector2f C{vertices[(i + 1) % vertices.size()]};
        areas.push_back(std::abs(cross(B, C)) / 2.f);
    }
    float totalArea{std::accumulate(areas.begin(), areas.end(), 0.f)};
    float momentOfInertia{0};
    for (std::size_t i{0}; i < vertices.size(); ++i) {
        Vector2f B{vertices[i]};
        Vector2f C{vertices[(i + 1) % vertices.size()]};
        float signedArea{cross(B, C) / 2.f};
        float triangleArea{areas[i]};
        float triangleMass{mass * triangleArea / totalArea};
        momentOfInertia += triangleMass * (signedArea / triangleArea) * (norm2(B) + norm2(C) + dot(B, C));
    }
    return (momentOfInertia / 6.f) - mass * norm2(centerOfMass);
}

Vector2f Collider::supportFunction(const Vector2f& direction, const Scene& scene, EntityId id) const {
    // For now, we only have convex bodies which need a support function,
    // so no dispatch like for Shadow.
    const Body& body{scene.getComponent<Body>(id)};
    const ConvexBody& convex{scene.getComponent<ConvexBody>(id)};

	float largestProd{-std::numeric_limits<float>::max()};
	Vector2f furthestPoint;
    bool found{false};
	for (Vector2f vertex : convex.vertices) {
		// Rotate the point around the center of mass to account for rotation
		Vector2f worldPoint{body.localToWorld(vertex)};

		float product{dot(direction, worldPoint)};
		if (product > largestProd) {
			furthestPoint = worldPoint;
			largestProd = product;
            found = true;
		}
	}
    assert(found); // This will fail if we have NaN as direction
    return furthestPoint;
}

std::pair<Vector2f, Vector2f> Shadow::shadowFunction(const Vector2f& lightSource, const Scene& scene, EntityId id) const {
    const Body& body{scene.getComponent<Body>(id)};
    switch(body.type) {
        case BodyType::Circle:
            return circleShadow(lightSource, body, scene.getComponent<CircleBody>(id));
            break;
        case BodyType::Convex:
            return convexShadow(lightSource, body, scene.getComponent<ConvexBody>(id));
            break;
        default:
            assert(false);
    }
}

std::pair<Vector2f, Vector2f> Shadow::circleShadow(const Vector2f& lightSource, const Body& body, const CircleBody& circle) const {
    // Get the left-side orthogonal vector
    Vector2f orthogonal{perpendicular(body.position - lightSource, true)};
    orthogonal /= norm(orthogonal);
    return {body.position + orthogonal * circle.radius, body.position - orthogonal * circle.radius};
}

std::pair<Vector2f, Vector2f> Shadow::convexShadow(const Vector2f& lightSource, const Body& body, const ConvexBody& convex) const {
    std::vector<float> angles(convex.vertices.size());
    std::vector<Vector2f> worldV;
    std::transform(convex.vertices.begin(), convex.vertices.end(), std::back_inserter(worldV),
        std::bind(&Body::localToWorld, &body, std::placeholders::_1));
    angles[0] = 0.f;
    const float angle0{angle(worldV[0] - lightSource)};
    for (std::size_t i{1}; i < worldV.size(); ++i) {
        angles[i] = angle(worldV[i] - lightSource) - angle0;
    }
    // Find which vertices have the greatest and smallest angles. Greater angle
    // means the rightmost respective to the light source (since the y-axis goes
    // down).
    auto pair = std::minmax_element(angles.begin(), angles.end());
    return {*(worldV.begin() + std::distance(angles.begin(), pair.first)),
            *(worldV.begin() + std::distance(angles.begin(), pair.second))};
}

void to_json(nlohmann::json& j, const Animations& animations) {
    for (auto& [action, animationData] : animations) {
		nlohmann::json key(action);
		j[key.get<std::string>()] = animationData;
	}
}

void from_json(const nlohmann::json& j, Animations& animations) {
    for (auto& [actionValue, animationDataValue] : j.items()) {
		Action action;
		nlohmann::json(actionValue).get_to(action);
		animationDataValue.get_to(animations[action]);
	}
}
