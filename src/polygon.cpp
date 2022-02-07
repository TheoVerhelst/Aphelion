#include <numeric>
#include <algorithm>
#include <polygon.hpp>
#include <components/Body.hpp>

ConvexPolygon::ConvexPolygon(const std::vector<Vector2f>& vertices):
    _vertices{vertices} {
}

Vector2f ConvexPolygon::supportFunction(const Vector2f& direction) const {
    std::vector<float> products(_vertices.size());
    std::transform(_vertices.begin(), _vertices.end(), products.begin(),
    [&direction] (const Vector2f& v) {
        return dot(direction, v);
    });
    auto it = std::max_element(products.begin(), products.end());
    assert(it != products.end()); // This will fail if we have NaN as direction
    return _vertices[std::distance(products.begin(), it)];
}


std::pair<float, Vector2f> ConvexPolygon::areaAndCenterOfMass() const {
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
	float area{std::accumulate(triangleAreas.begin(), triangleAreas.end(), 0.f)};
	Vector2f centerOfMass{std::inner_product(triangleCenters.begin(),
		triangleCenters.end(), triangleAreas.begin(), Vector2f(0.f, 0.f))};
	return {area, centerOfMass / area};
}

float ConvexPolygon::momentOfInertia(float density, const Vector2f& axis) const {
    float momentOfInertia{0};
    float area{0};
    const Vector2f A{_vertices[0]};
    for (std::size_t i{1}; i < _vertices.size() - 1; ++i) {
        const Vector2f B{_vertices[i] - A};
        const Vector2f C{_vertices[i + 1] - A};
        // Vertices are in clockwise order, so the correct formula is C x B. But
        // the y-axis is going down, so we must invert it once more (clockwise
        // order in normal axis is counterclockwise in inverted y-axis).
        const float signedArea{cross(B, C) / 2.f};
        area += std::abs(signedArea);
        momentOfInertia += signedArea * (norm2(B) + norm2(C) + dot(B, C));
    }
    return (momentOfInertia  * density / 6.f) - (area * density) * norm2(axis - A);
}

bool ConvexPolygon::contains(const Vector2f& P) const {
	for (std::size_t i{0}; i < _vertices.size(); ++i) {
		const std::size_t j{(i + 1) % _vertices.size()};
		const std::size_t k{(i + 2) % _vertices.size()};
		if (cross(_vertices[j] - _vertices[i], P - _vertices[i])
		  * cross(_vertices[j] - _vertices[i], _vertices[k] - _vertices[i]) < 0) {
			return false;
		}
	}
	return true;
}

bool boxContains(const std::array<Vector2f, 4>& box, const Vector2f& P) {
	//  A -------- B
	//  |   P      |
	//  D -------- C
	const Vector2f AB{box[1] - box[0]};
	const Vector2f AP{P      - box[0]};
	const Vector2f BP{P      - box[1]};
	const Vector2f AD{box[3] - box[0]};
	const Vector2f DP{P      - box[3]};
	return dot(AB, AP) >= 0 and dot(AB, BP) <= 0
	   and dot(AD, AP) >= 0 and dot(AD, DP) <= 0;
}


std::vector<std::vector<std::size_t>> earClipping(const std::vector<Vector2f>& vertices) {
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
                if (ConvexPolygon({A, B, C}).contains(D)) {
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

std::vector<std::vector<std::size_t>> HertelMehlhorn(
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
