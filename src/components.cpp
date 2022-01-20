#include <algorithm>
#include <SFML/Graphics/RenderTarget.hpp>
#include <components.hpp>

Vector2d Body::localToWorld(const Vector2d& point) const {
    return rotate(point, rotation) + position;
}

Vector2d Body::worldToLocal(const Vector2d& point) const {
    return rotate(point - position, -rotation);
}

Vector2d CircleBody::computeCenterOfMass() const {
    return {radius, radius};
}

double CircleBody::computeMomentOfInertia(double mass) const {
    return mass * radius * radius / 2.;
}

Vector2d ConvexBody::computeCenterOfMass() const {
	if (vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
	std::vector<double> triangleAreas;
	std::vector<Vector2d> triangleCenters;
    const Vector2d A{vertices[0]};
	for (std::size_t i{1}; i < vertices.size() - 1; ++i) {
		Vector2d B{vertices[i]};
		Vector2d C{vertices[i + 1]};
		triangleAreas.push_back(std::abs(cross(B - A, C - A)) / 2.);
		triangleCenters.push_back((A + B + C) / 3.);
	}
	double totalArea{std::accumulate(triangleAreas.begin(), triangleAreas.end(), 0.)};
	Vector2d centerOfMass{std::inner_product(triangleCenters.begin(),
		triangleCenters.end(), triangleAreas.begin(), Vector2d(0, 0))};
	return centerOfMass / totalArea;
}

double ConvexBody::computeMomentOfInertia(double mass, const Vector2d& centerOfMass) const {
	if (vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
    std::vector<double> areas;
    for (std::size_t i{0}; i < vertices.size(); ++i) {
        Vector2d B{vertices[i]};
        Vector2d C{vertices[(i + 1) % vertices.size()]};
        areas.push_back(std::abs(cross(B, C)) / 2.);
    }
    double totalArea{std::accumulate(areas.begin(), areas.end(), 0.)};
    double momentOfInertia{0};
    for (std::size_t i{0}; i < vertices.size(); ++i) {
        Vector2d B{vertices[i]};
        Vector2d C{vertices[(i + 1) % vertices.size()]};
        double signedArea{cross(B, C) / 2.};
        double triangleArea{areas[i]};
        double triangleMass{mass * triangleArea / totalArea};
        momentOfInertia += triangleMass * (signedArea / triangleArea) * (norm2(B) + norm2(C) + dot(B, C));
    }
    return (momentOfInertia / 6.) - mass * norm2(centerOfMass);
}

Vector2d Collider::supportFunction(const Vector2d& direction, const Scene& scene, EntityId id) const {
    // For now, we only have convex bodies which need a support function,
    // so no dispatch like for Shadow.
    const Body& body{scene.getComponent<Body>(id)};
    const ConvexBody& convex{scene.getComponent<ConvexBody>(id)};

	double largestProd{-std::numeric_limits<double>::max()};
	Vector2d furthestPoint;
    bool found{false};
	for (Vector2d vertex : convex.vertices) {
		// Rotate the point around the center of mass to account for rotation
		Vector2d worldPoint{body.localToWorld(vertex)};

		double product{dot(direction, worldPoint)};
		if (product > largestProd) {
			furthestPoint = worldPoint;
			largestProd = product;
            found = true;
		}
	}
    assert(found); // This will fail if we have NaN as direction
    return furthestPoint;
}

std::pair<Vector2d, Vector2d> Shadow::shadowFunction(const Vector2d& lightSource, const Scene& scene, EntityId id) const {
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

std::pair<Vector2d, Vector2d> Shadow::circleShadow(const Vector2d& lightSource, const Body& body, const CircleBody& circle) const {
    // Get the left-side orthogonal vector
    Vector2d orthogonal{perpendicular(body.position - lightSource, true)};
    orthogonal /= norm(orthogonal);
    return {body.position + orthogonal * circle.radius, body.position - orthogonal * circle.radius};
}

std::pair<Vector2d, Vector2d> Shadow::convexShadow(const Vector2d& lightSource, const Body& body, const ConvexBody& convex) const {
    std::vector<double> angles(convex.vertices.size());
    std::vector<Vector2d> worldV;
    std::transform(convex.vertices.begin(), convex.vertices.end(), std::back_inserter(worldV),
        std::bind(&Body::localToWorld, &body, std::placeholders::_1));
    angles[0] = 0;
    const double angle0{angle(worldV[0] - lightSource)};
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

void Trace::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(trace, states);
}

void AnimationComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& [acion, animation] : animations) {
        if(not animation.isStopped()) {
            target.draw(animation, states);
        }
    }
}
