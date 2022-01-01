#include <stdexcept>
#include <iostream>
#include <collisions.hpp>

void collideBodies(Body& bodyA, Body& bodyB) {
    std::optional<impl::MinkowskyPolygon> collision{impl::GJK(bodyA, bodyB)};
    if(collision.has_value()) {
        impl::ContactInfo contactInfo{impl::EPA(bodyA, bodyB, collision.value())};
        double totalMass{bodyA.getMass() + bodyB.getMass()};
        // n is the normalized vector normal to the surface of contact
        Vector2d n{contactInfo.normal / norm(contactInfo.normal)};
        std::cout << "ContactInfo:" << std::endl;
        std::cout << "Class A: " << typeid(bodyA).name() << std::endl;
        std::cout << "Class B: " << typeid(bodyB).name() << std::endl;
        std::cout << "A.position = " << bodyA.getPosition() << std::endl;
        std::cout << "B.position = " << bodyB.getPosition() << std::endl;
        std::cout << "C_A = " << contactInfo.C_A << std::endl;
        std::cout << "C_B = " << contactInfo.C_B << std::endl;
        std::cout << "n = " << n << std::endl;
        std::cout << "|normal| = " << norm(contactInfo.normal) << std::endl;

        Vector2d R_A{contactInfo.C_A - bodyA.getCenterOfMass() - bodyA.getPosition()};
        Vector2d R_B{contactInfo.C_B - bodyB.getCenterOfMass() - bodyB.getPosition()};
        Vector2d v_A{bodyA.getVelocity()};
        Vector2d v_B{bodyB.getVelocity()};
        double m_A{bodyA.getMass()};
        double m_B{bodyB.getMass()};
        double I_A{bodyA.getMomentOfInertia()};
        double I_B{bodyB.getMomentOfInertia()};
        double w_A{bodyA.getAngularVelocity()};
        double w_B{bodyB.getAngularVelocity()};
        double R_A_n{cross(R_A, n)};
        double R_B_n{cross(R_B, n)};

        // norm_J is the norm of the resulting impulse vector
        double norm_J{- 2 *
            ( m_A * dot(v_B, n)
            - m_B * dot(v_A, n)
            + m_A * m_B * w_B * R_B_n
            - m_A * m_B * w_A * R_A_n) /
            ( m_A + m_B
            + m_A * m_B * R_A_n * R_A_n / I_A
            + m_A * m_B * R_B_n * R_B_n / I_B)
        };
        Vector2d J{n * norm_J};

        bodyA.setVelocity(v_A - J / m_A);
        bodyB.setVelocity(v_B + J / m_B);
        bodyA.setAngularVelocity(w_A - cross(R_A, J) / I_A);
        bodyB.setAngularVelocity(w_B + cross(R_B, J) / I_B);

        // Shift the bodies out of collision
        bodyA.setPosition(bodyA.getPosition() - contactInfo.normal * bodyB.getMass() / totalMass);
        bodyB.setPosition(bodyB.getPosition() + contactInfo.normal * bodyA.getMass() / totalMass);
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

void MinkowskyPolygon::pushBack(const Vector2d& A, const Vector2d& B) {
    insert(size(), A, B);
}

void MinkowskyPolygon::insert(std::size_t index, const Vector2d& A, const Vector2d& B) {
    _pointsA.insert(_pointsA.begin() + index, A);
    _pointsB.insert(_pointsB.begin() + index, B);
}

void MinkowskyPolygon::erase(std::size_t index) {
    _pointsA.erase(_pointsA.begin() + index);
    _pointsB.erase(_pointsB.begin() + index);
}

Vector2d MinkowskyPolygon::getDifference(std::size_t index) const {
    return getPointA(index) - getPointB(index);
}

Vector2d MinkowskyPolygon::getPointA(std::size_t index) const {
    return _pointsA.at(index);
}

Vector2d MinkowskyPolygon::getPointB(std::size_t index) const {
    return _pointsB.at(index);
}

std::size_t MinkowskyPolygon::size() const {
    return _pointsA.size();
}

std::optional<MinkowskyPolygon> GJK(const Body& bodyA, const Body& bodyB) {
    // GJK algorithm, see https://blog.winter.dev/2020/gjk-algorithm/
	// Or even better: https://youtu.be/ajv46BSqcK4
    Vector2d direction{1, 0};
    Vector2d supportA{bodyA.support(direction)};
    Vector2d supportB{bodyB.support(-direction)};
    Vector2d supportPoint{supportA - supportB};
    MinkowskyPolygon simplex;
    simplex.pushBack(supportA, supportB);

    direction = -supportPoint;
    std::size_t maxIter{50};
    for (std::size_t i{0}; i < maxIter; ++i) {
        supportA = bodyA.support(direction);
        supportB = bodyB.support(-direction);
        supportPoint = supportA - supportB;
        if (dot(supportPoint, direction) < 0) {
            // No collision
            return std::optional<MinkowskyPolygon>();
        }
        simplex.pushBack(supportA, supportB);
        if(impl::nearestSimplex(simplex, direction)) {
            return simplex;
        }
    }
    return std::optional<MinkowskyPolygon>();
}

ContactInfo EPA(const Body& bodyA, const Body& bodyB, MinkowskyPolygon polygon) {
    double eps{0.001};
    double collisionGap{0.001};
    std::size_t maxIter{50};

    double minDistance;
    Vector2d minNormal;
    std::size_t minIndex;

    for (std::size_t t{0}; t < maxIter; ++t) {
        // Find the polygon edge closest to the origin, and the associated
        // normal vector pointing away from the origin
        minDistance = std::numeric_limits<double>::max();
        for (std::size_t i{0}; i < polygon.size(); ++i) {
            std::size_t j{(i + 1) % polygon.size()};
            Vector2d A{polygon.getDifference(i)}, B{polygon.getDifference(j)};
            Vector2d normal{perpendicular(B - A, A)};
            normal /= norm(normal);
            double distance{dot(normal, A)};
            if (distance < minDistance) {
                minDistance = distance;
                minNormal = normal;
                minIndex = i;
            }
        }
        // Find the support point in the direction of the normal, and check
        // if this point is further
        Vector2d supportA{bodyA.support(minNormal)};
        Vector2d supportB{bodyB.support(-minNormal)};
        double supportDistance{dot(supportA - supportB, minNormal)};

        // If the point is no further, we found the closest edge.
        // We also stop here if we are at the end of the loop
        if (std::abs(supportDistance - minDistance) <= eps or t == maxIter - 1) {
            // Find the four points in A and B which correspond to the current
            // edge of the the polygon
            std::size_t j{(minIndex + 1) % polygon.size()};
            Vector2d A_i{polygon.getPointA(minIndex)}, A_j{polygon.getPointA(j)};
            Vector2d B_i{polygon.getPointB(minIndex)}, B_j{polygon.getPointB(j)};
            Vector2d S_i{A_i - B_i}, S_j{A_j - B_j};
            // Alpha is the barycentric coordinate between S_i and S_j
            // of the vector projection of the origin onto the line S_i S_J.
            // So if alpha = 0, then the origin maps to S_i, and if alpha = 1,
            // it maps to S_j
            double alpha{dot(S_j - S_i, -S_i) / norm2(S_j - S_i)};

            ContactInfo info;
            info.normal = minNormal * (minDistance + collisionGap);
            info.C_A = A_i * (1 - alpha) + A_j * alpha;
            info.C_B = B_i * (1 - alpha) + B_j * alpha;
            return info;
        }
        // Otherwise, add the point to the polygon
        polygon.insert((minIndex + 1) % polygon.size(), supportA, supportB);
    }
    return ContactInfo();
}

bool nearestSimplex(MinkowskyPolygon& simplex, Vector2d& direction) {
    if (simplex.size() == 2) {
        return simplexLine(simplex, direction);
    } else if (simplex.size() == 3) {
        return simplexTriangle(simplex, direction);
    } else {
        throw std::runtime_error("Invalid simplex size");
    }
    return false;
}

bool simplexLine(MinkowskyPolygon& simplex, Vector2d& direction) {
    Vector2d A{simplex.getDifference(1)}, B{simplex.getDifference(0)};
    // Vector going from the new point to the old point in the simplex
    Vector2d AB{B - A};
    // New direction is perpendicular to this vector (towards origin)
    direction = perpendicular(AB, -A);
    return false;
}

bool simplexTriangle(MinkowskyPolygon& simplex, Vector2d& direction) {
    Vector2d A{simplex.getDifference(2)}, B{simplex.getDifference(1)}, C{simplex.getDifference(0)};
	Vector2d AC{C - A}, AB{B - A};
    // Check if we are outside the AC face. We go away from cb to stay outside
    // the simplex.
    Vector2d AC_perp{perpendicular(AC, -AB)};
    // If the origin is on this side
    if (dot(AC_perp, -A) > 0) {
		// Remove B from the simplex
        simplex.erase(1);
        direction = AC_perp;
        return false;
    }
    Vector2d AB_perp{perpendicular(AB, -AC)};
    if (dot(AB_perp, -A) > 0) {
		// Remove C from the simplex
        simplex.erase(0);
        direction = AB_perp;
        return false;
    }
    // Must be inside the simplex
    return true;
}

} // namespace impl
