#include <stdexcept>
#include <collisions.hpp>

void collideBodies(Body& bodyA, Body& bodyB) {
    if(GJK(bodyA, bodyB)) {
        // TODO response
    }
}

std::optional<CollisionResponse> GJK(const Body& bodyA, const Body& bodyB) {
    // GJK algorithm, see https://blog.winter.dev/2020/gjk-algorithm/
	// Or even better: https://youtu.be/ajv46BSqcK4
    Vector2d direction{1, 0};
	Vector2d A{bodyA.support(direction)};
	Vector2d B{bodyB.support(-direction)};
    Vector2d supportPoint{A - B};
    std::vector<std::pair<Vector2d, Vector2d>> simplexPoints{{A, B}};
    std::vector<Vector2d> simplex{supportPoint};

    direction = -supportPoint;
    while (true) {
		A = bodyA.support(direction);
		B = bodyB.support(-direction);
        supportPoint = A - B;
        if (dot(supportPoint, direction) < 0) {
            // No collision
            return std::optional<CollisionResponse>();
        }
		simplexPoints.push_back({A, B});
        simplex.push_back(supportPoint);
        if(impl::nearestSimplex(simplex, direction, simplexPoints)) {
			// The simplex must contain 3 points. We check to which side the
			// origin is closest to. It must be one of the two sides containing
			// the last point.
			CollisionResponse response;
			Vector2d S_0{simplex.at(0)}, S_1{simplex.at(1)}, S_2{simplex.at(2)};
            const auto& [A_2, B_2] = simplexPoints.at(2);
			if (dot(S_1 - S_2, -S_2) > 0) {
				// We are closest to segment S_1 S_2.
				const auto& [A_1, B_1] = simplexPoints.at(1);
                return {{A_1, B_1, B_2, true}};
            } else {
				// We are closest to segment S_0 S_2.
				const auto& [A_0, B_0] = simplexPoints.at(0);
                return {{A_0, A_2, B_0, false}};
            }
            return std::optional<CollisionResponse>();
        }
    }
}

void collideCircles(CircleBody& bodyA, CircleBody& bodyB) {
    const Vector2d diff_x = bodyB.getPosition() - bodyA.getPosition();
    const double dist{norm(diff_x)};
    const double overlap{bodyA.getRadius() + bodyB.getRadius() - dist};

    // Collision
    if (overlap > 0) {
        // Distance squared
        const double dist_2{norm2(diff_x)};
        const Vector2d v_a = bodyA.getVelocity();
        const Vector2d v_b = bodyB.getVelocity();
        const double m_a = bodyA.getMass();
        const double m_b = bodyB.getMass();
        // Based on stackoverflow.com/questions/35211114/2d-elastic-ball-collision-physics
        const Vector2d addedVel = 2 * dot(v_a - v_b, diff_x) * diff_x / ((m_a + m_b) * dist_2);
        bodyA.setVelocity(v_a - addedVel * m_b);
        bodyB.setVelocity(v_b + addedVel * m_a);

        // Move the bodies so that they just touch and don't overlap.
        // The displacement is proportional to the mass of the other body.
        bodyA.setPosition(bodyA.getPosition() - m_b * overlap * diff_x / (dist * (m_a + m_b)));
        bodyB.setPosition(bodyB.getPosition() + m_a * overlap * diff_x / (dist * (m_a + m_b)));
    }
}

namespace impl {

bool nearestSimplex(std::vector<Vector2d>& simplex, Vector2d& direction,
		std::vector<std::pair<Vector2d, Vector2d>>& simplexPoints) {
    if (simplex.size() == 2) {
        return simplexLine(simplex, direction);
    } else if (simplex.size() == 3) {
        return simplexTriangle(simplex, direction, simplexPoints);
    } else {
        throw std::runtime_error("Invalid simplex size");
    }
    return false;
}

bool simplexLine(std::vector<Vector2d>& simplex, Vector2d& direction) {
    Vector2d A{simplex.at(1)}, B{simplex.at(0)};
    // Vector going from the new point to the old point in the simplex
    Vector2d AB{B - A};
    // New direction is perpendicular to this vector (towards origin)
    direction = perpendicular(AB, -A);
    return false;
}

bool simplexTriangle(std::vector<Vector2d>& simplex, Vector2d& direction,
		std::vector<std::pair<Vector2d, Vector2d>>& simplexPoints) {
    Vector2d A{simplex.at(2)}, B{simplex.at(1)}, C{simplex.at(0)};
	Vector2d AC{C - A}, AB{B - A};
    // Check if we are outside the AC face. We go away from cb to stay outside
    // the simplex.
    Vector2d AC_perp{perpendicular(AC, -AB)};
    // If the origin is on this side
    if (dot(AC_perp, -A) > 0) {
		// Remove B from the simplex
        simplex = {C, A};
		simplexPoints = {simplexPoints.at(0), simplexPoints.at(2)};
        direction = AC_perp;
        return false;
    }
    Vector2d AB_perp{perpendicular(AB, -AC)};
    if (dot(AB_perp, -A) > 0) {
		// Remove C from the simplex
        simplex = {B, A};
		simplexPoints = {simplexPoints.at(1), simplexPoints.at(2)};
        direction = AB_perp;
        return false;
    }
    // Must be inside the simplex
    return true;
}

} // namespace impl
