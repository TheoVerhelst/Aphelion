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
    body.mass = 2 * pi * radius * radius * body.density;
    body.centerOfMass = {radius, radius};
    body.momentOfInertia = body.mass * radius * radius / 2.f;
}

std::vector<Vector2f> CircleBody::shadowTerminator(const Vector2f& lightSource, const Body& body) const {
    // Get the left-sided normal vector
    Vector2f orthogonal{perpendicular(body.position - lightSource, true)};
    orthogonal /= norm(orthogonal);
    return {body.position + orthogonal * radius, body.position - orthogonal * radius};
}

ConvexComponent::ConvexComponent(const std::vector<Vector2f>& vertices):
    _vertices{vertices} {
    computeAreaAndCenterOfMass();
}

float ConvexComponent::computeMomentOfInertia(float density, const Vector2f& axis) const {
    float momentOfInertia{0};
    for (std::size_t i{0}; i < _vertices.size(); ++i) {
        const Vector2f B{_vertices[i]};
        const Vector2f C{_vertices[(i + 1) % _vertices.size()]};
        momentOfInertia += cross(B, C) * (norm2(B) + norm2(C) + dot(B, C));
    }
    return (momentOfInertia  * density / 12.f) - (_area * density) * norm2(axis);
}

Vector2f ConvexComponent::supportFunction(const Vector2f& direction, const Body& body) const {
	float largestProd{-std::numeric_limits<float>::max()};
	Vector2f furthestPoint;
    bool found{false};
	for (Vector2f vertex : _vertices) {
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

float ConvexComponent::getArea() const {
    return _area;
}

Vector2f ConvexComponent::getCenterOfMass() const {
    return _centerOfMass;
}

void ConvexComponent::updateCenterOfMass(const Vector2f& centerOfMass) {
    for (Vector2f& v : _vertices) {
        v -= centerOfMass;
    }
}

void ConvexComponent::computeAreaAndCenterOfMass() {
	if (_vertices.size() < 3) {
		throw std::runtime_error("Invalid shape");
	}
	std::vector<float> triangleAreas;
	std::vector<Vector2f> triangleCenters;
    const Vector2f A{_vertices[0]};
	for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
		const Vector2f B{_vertices[i]};
		const Vector2f C{_vertices[i + 1]};
		triangleAreas.push_back(std::abs(cross(B - A, C - A)) / 2.f);
		triangleCenters.push_back((A + B + C) / 3.f);
	}
	_area = std::accumulate(triangleAreas.begin(), triangleAreas.end(), 0.f);
	_centerOfMass = std::inner_product(triangleCenters.begin(),
		triangleCenters.end(), triangleAreas.begin(), Vector2f(0.f, 0.f));
	_centerOfMass /= _area;
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
    for (ConvexComponent& component : components) {
        area += component.getArea();
        body.centerOfMass += component.getArea() * component.getCenterOfMass();
    }
    body.centerOfMass /= area;
    body.mass = area * body.density;
    body.momentOfInertia = 0;
    for (ConvexComponent& component : components) {
        body.momentOfInertia += component.computeMomentOfInertia(body.density, body.centerOfMass);
    }
    for (ConvexComponent& component : components) {
        component.updateCenterOfMass(body.centerOfMass);
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

std::vector<std::vector<std::size_t>> PolygonBody::earClipping(const std::vector<Vector2f>& vertices) {
    std::vector<std::size_t> indices(vertices.size());
    // Fill indices with 0, 1, ..., vertices.size()
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<std::vector<std::size_t>> triangulation;
    std::size_t i{0};
    while (indices.size() > 3) {
        const std::size_t j{(i + 1) % indices.size()};
        const std::size_t k{(i + 2) % indices.size()};
        const Vector2f A{vertices[indices[i]]};
        const Vector2f B{vertices[indices[j]]};
        const Vector2f C{vertices[indices[k]]};
        if (angle(A, B, C) < pi) {
            bool isEar{true};
            // Check if any of the other vertices is inside ABC
            for (std::size_t l{0}; l < indices.size() - 3; ++l) {
                const Vector2f D{vertices[indices[(k + 1 + l) % indices.size()]]};
                if (convexContains({A, B, C}, D)) {
                    isEar = false;
                    break;
                }
            }
            if (isEar) {
                triangulation.push_back({indices[i], indices[j], indices[k]});
                indices.erase(indices.begin() + j);
            }
        }
        i = (i + 1) % indices.size();
    }
    assert(indices.size() == 3);
    triangulation.push_back(indices);
    return triangulation;
}

std::vector<std::vector<std::size_t>> PolygonBody::HertelMehlhorn(
        const std::vector<Vector2f>& vertices,
        std::vector<std::vector<std::size_t>> components) {
    // Iterate on the components
    for (std::size_t compIdxA{0}; compIdxA < components.size(); ++compIdxA) {
        std::vector<std::size_t>& componentA{components[compIdxA]};
        // Iterate over the vertices in component A (in clockwise order)
        //
        // A2 *-----------* A3
        //    |
        //    |  <- edge to check
        //    |
        // A1 *-------* A0 (iA)
        for (std::size_t iA{0}; iA < componentA.size(); ++iA) {
            const std::size_t A0{componentA[iA]};
            const std::size_t A1{componentA[(iA + 1) % componentA.size()]};
            const std::size_t A2{componentA[(iA + 2) % componentA.size()]};
            const std::size_t A3{componentA[(iA + 3) % componentA.size()]};
            // Check if the edge 1-2 is a triangulation edge
            if ((A2 - A1) % vertices.size() != 1) {
                // Find the adjacent component
                for (std::size_t compIdxB{0}; compIdxB < components.size(); ++compIdxB) {
                    if (compIdxA != compIdxB) {
                        const std::vector<std::size_t>& componentB{components[compIdxB]};
                        // Iterate over the vertices in B
                        // (iB)   B1 A2
                        // B0 *-----*-----------* A3
                        //          |
                        //          |  <- edge to check
                        //          |
                        // B3 *-----*-------* A0
                        //        B2 A1
                        //
                        for (std::size_t iB{0}; iB < componentB.size(); ++iB) {
                            const std::size_t B0{componentB[iB]};
                            const std::size_t B1{componentB[(iB + 1) % componentB.size()]};
                            const std::size_t B2{componentB[(iB + 2) % componentB.size()]};
                            const std::size_t B3{componentB[(iB + 3) % componentB.size()]};

                            // Check if the vertex is adjacent, and if it is
                            // essential. The vertex order in inverted because the
                            // edges in B and A go clockwise.
                            if (A1 == B2 and B1 == A2 and
                                    angle(vertices[A0], vertices[A1], vertices[A2]) +
                                    angle(vertices[B1], vertices[B2], vertices[B3]) <= pi and
                                    angle(vertices[A1], vertices[A2], vertices[A3]) +
                                    angle(vertices[B0], vertices[B1], vertices[B2]) <= pi) {
                                // Insert the vertices from B into A, going
                                // backwards but inserting at a fixed position
                                // in A (so they end up in the correct order)
                                std::size_t idxA{(iA + 2) % componentA.size()};
                                for (std::size_t jB{0}; jB < componentB.size() - 2; ++jB) {
                                    std::size_t idxB{(iB + componentB.size() - jB) % componentB.size()};
                                    componentA.insert(componentA.begin() + idxA, componentB[idxB]);
                                }
                                components.erase(components.begin() + compIdxB);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return components;
}
