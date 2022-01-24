#include <stdexcept>
#include <systems/CollisionSystem.hpp>

using namespace std::placeholders;

CollisionSystem::CollisionSystem(Scene& scene):
    _scene{scene} {
}

void CollisionSystem::update(const sf::Time&) {
    std::vector<EntityId> colliderView{_scene.view<Body, Collider>()};
    std::vector<EntityId> circleView{_scene.view<Body, CircleBody>()};

    for (std::size_t i{0}; i < circleView.size(); ++i) {
        Body& bodyA{_scene.getComponent<Body>(circleView[i])};
        const CircleBody& circleA{_scene.getComponent<CircleBody>(circleView[i])};

        // Circle - circle collisions
        for (std::size_t j{i + 1}; j < circleView.size(); ++j) {
            Body& bodyB{_scene.getComponent<Body>(circleView[j])};
            const CircleBody& circleB{_scene.getComponent<CircleBody>(circleView[j])};
            collideCircles(circleA, circleB, bodyA, bodyB);
        }

        // Circle - arbitrary body collisions
        for (std::size_t j{0}; j < colliderView.size(); ++j) {
            Body& bodyB{_scene.getComponent<Body>(colliderView[j])};
            const Collider& colliderB{_scene.getComponent<Collider>(colliderView[j])};
            SupportFunction functionB{std::bind(&Collider::supportFunction, &colliderB, _1, std::cref(_scene), colliderView[j])};
            collideCircleAndBody(circleA, functionB, bodyA, bodyB);
        }
    }

    for (std::size_t i{0}; i < colliderView.size(); ++i) {
        Body& bodyA{_scene.getComponent<Body>(colliderView[i])};
        const Collider& colliderA{_scene.getComponent<Collider>(colliderView[i])};
        SupportFunction functionA{std::bind(&Collider::supportFunction, &colliderA, _1, std::cref(_scene), colliderView[i])};

        // arbitrary body - body collisions
        for (std::size_t j{i + 1}; j < colliderView.size(); ++j) {
            Body& bodyB{_scene.getComponent<Body>(colliderView[j])};
            const Collider& colliderB{_scene.getComponent<Collider>(colliderView[j])};
            SupportFunction functionB{std::bind(&Collider::supportFunction, &colliderB, _1, std::cref(_scene), colliderView[j])};
            collideBodies(functionA, functionB, bodyA, bodyB);
        }
    }
}

void CollisionSystem::collideBodies(const SupportFunction& functionA,
        const SupportFunction& functionB, Body& bodyA, Body& bodyB) {
    // Determine if the bodies collide with GJK
    std::pair<bool, MinkowskyPolygon> collision{collisionGJK(functionA, functionB)};
    if (collision.first) {
        // Determine the collision info with EPA
        ContactInfo contactInfo{EPA(functionA, functionB, collision.second)};
        // Update the bodies' speed, position and rotation according to the collision
        collisionResponse(bodyA, bodyB, contactInfo);
    }
}

void CollisionSystem::collideCircles(const CircleBody& circleA,
        const CircleBody& circleB, Body& bodyA, Body& bodyB) {
    const Vector2d diff_x = bodyB.position - bodyA.position;
    const double dist{norm(diff_x)};
    const double overlap{circleA.radius + circleB.radius - dist};

    // Collision
    if (overlap > 0) {
        // Distance squared
        const Vector2d v_a{bodyA.velocity};
        const Vector2d v_b{bodyB.velocity};
        const double m_a{bodyA.mass};
        const double m_b{bodyB.mass};
        const Vector2d normal{diff_x / dist};
        const double restitution{bodyA.restitution * bodyB.restitution};
        const Vector2d addedVel = dot(v_a - v_b, normal) * normal / (m_a + m_b);
        bodyA.velocity -= addedVel * (m_b + restitution * m_b);
        bodyB.velocity += addedVel * (m_a + restitution * m_a);

        // Move the bodies so that they just touch and don't overlap.
        // The displacement is proportional to the mass of the other body.
        bodyA.position -= m_b * overlap * diff_x / (dist * (m_a + m_b));
        bodyB.position += m_a * overlap * diff_x / (dist * (m_a + m_b));
    }
}

void CollisionSystem::collideCircleAndBody(const CircleBody& circleA,
        const SupportFunction& functionB, Body& bodyA, Body& bodyB) {
    // Check the distance between B and the center of A
    const Vector2d centerA{bodyA.localToWorld({0, 0})};
    SupportFunction functionA = [centerA](const Vector2d&) noexcept {return centerA;};
    std::pair<bool, MinkowskyPolygon> collision{collisionGJK(functionA, functionB)};
    // If the center of A is not in B
    if (not collision.first) {
        // Find the distance between B and the center of A
        ContactInfo distanceInfo{distanceGJK(functionA, functionB, collision.second)};
        // If the distance is greater than the radius, we are fine. Otherwise,
        // we need to solve the collision by translating the distance info to a
        // contact info between the circle A and the body B. We use an epsilon
        // value to avoid detecting a collision when the overlap is not big
        // enough.
        if (circleA.radius - distanceInfo.distance > eps) {
            // Normalized vector from the closest point on B to the center of A
            Vector2d normal{distanceInfo.normal / distanceInfo.distance};

            ContactInfo contactInfo{distanceInfo.C_B, centerA + normal * circleA.radius};
            collisionResponse(bodyA, bodyB, contactInfo);
        }
    } else {
        // The center of A is in B, use EPA to find the collision vector, and
        // increase it to clear the whole circle A from B.
        ContactInfo contactInfo{EPA(functionA, functionB, collision.second)};
        contactInfo.normal += contactInfo.normal * circleA.radius / norm(contactInfo.normal);
        collisionResponse(bodyA, bodyB, contactInfo);
    }
}

void CollisionSystem::collisionResponse(Body& bodyA, Body& bodyB, const ContactInfo& contactInfo) {
    // Vector going from the center of mass to the contact point.
    // We don't use Body::worldToLocal because we need to keep the angle.
    const Vector2d R_A{contactInfo.C_A - bodyA.position};
    const Vector2d R_B{contactInfo.C_B - bodyB.position};
    const Vector2d v_A{bodyA.velocity};
    const Vector2d v_B{bodyB.velocity};
    const double m_A{bodyA.mass};
    const double m_B{bodyB.mass};
    const double I_A{bodyA.momentOfInertia};
    const double I_B{bodyB.momentOfInertia};
    const double w_A{bodyA.angularVelocity};
    const double w_B{bodyB.angularVelocity};
    // n is the normalized vector normal to the surface of contact
    const Vector2d n{contactInfo.normal / norm(contactInfo.normal)};
    const double R_A_n{cross(R_A, n)};
    const double R_B_n{cross(R_B, n)};
    const double restitution{bodyA.restitution * bodyB.restitution};

    // norm_J is the norm of the resulting impulse vector
    const double norm_J_elastic{2 *
        (dot(v_A - v_B, n) + cross(w_A * R_A - w_B * R_B, n)) /
        (1 / m_A + 1 / m_B + R_A_n * R_A_n / I_A + R_B_n * R_B_n / I_B)
    };
    const double norm_J_inelastic{m_A * m_B * dot(v_A - v_B, n) / (m_A + m_B)};
    Vector2d J{n * (restitution * norm_J_elastic + (1 - restitution) * norm_J_inelastic)};

    // Add friction if necessary. First, let's compute the normalized tangent
    // vector, orthogonal to the normal vector.
    const Vector2d t{perpendicular(n, true)};
    const double relativeVelocity{dot(v_A - v_B, t) - w_A * norm(R_A) - w_B * norm(R_B)};
    if (std::abs(relativeVelocity) >= eps) {
        J += t * bodyA.friction * bodyB.friction * norm(J) * relativeVelocity / std::abs(relativeVelocity);
    }

    bodyA.velocity -= J / m_A;
    bodyB.velocity += J / m_B;
    bodyA.angularVelocity -= cross(R_A, J) / I_A;
    bodyB.angularVelocity += cross(R_B, J) / I_B;

    // Shift the bodies out of collision
    bodyA.position -= contactInfo.normal * bodyB.mass / (bodyA.mass + bodyB.mass);
    bodyB.position += contactInfo.normal * bodyA.mass / (bodyA.mass + bodyB.mass);
}

void CollisionSystem::MinkowskyPolygon::pushBack(const Vector2d& A, const Vector2d& B) {
    insert(size(), A, B);
}

void CollisionSystem::MinkowskyPolygon::insert(std::size_t index, const Vector2d& A, const Vector2d& B) {
    _pointsA.insert(_pointsA.begin() + index, A);
    _pointsB.insert(_pointsB.begin() + index, B);
}

void CollisionSystem::MinkowskyPolygon::erase(std::size_t index) {
    _pointsA.erase(_pointsA.begin() + index);
    _pointsB.erase(_pointsB.begin() + index);
}

Vector2d CollisionSystem::MinkowskyPolygon::getDifference(std::size_t index) const {
    return getPointA(index) - getPointB(index);
}

Vector2d CollisionSystem::MinkowskyPolygon::getPointA(std::size_t index) const {
    return _pointsA.at(index);
}

Vector2d CollisionSystem::MinkowskyPolygon::getPointB(std::size_t index) const {
    return _pointsB.at(index);
}

std::size_t CollisionSystem::MinkowskyPolygon::size() const {
    return _pointsA.size();
}

CollisionSystem::ContactInfo::ContactInfo(const Vector2d& A, const Vector2d& B):
    C_A{A},
    C_B{B},
    normal{B - A}, // TODO THIS SHOULD BE A - B, WHATS GOING ON HERE
    distance{norm(normal)} {
    assert(A != B);
}

std::pair<bool, CollisionSystem::MinkowskyPolygon> CollisionSystem::collisionGJK(
            const SupportFunction& functionA, const SupportFunction& functionB) {
    // GJK algorithm, see https://blog.winter.dev/2020/gjk-algorithm/
	// Or even better: https://youtu.be/ajv46BSqcK4
    Vector2d direction{1, 0};
    Vector2d supportA{functionA(direction)};
    Vector2d supportB{functionB(-direction)};
    Vector2d supportPoint{supportA - supportB};
    MinkowskyPolygon simplex;
    simplex.pushBack(supportA, supportB);

    direction = -supportPoint;
    std::size_t maxIter{50};
    for (std::size_t i{0}; i < maxIter; ++i) {
        supportA = functionA(direction);
        supportB = functionB(-direction);
        supportPoint = supportA - supportB;
        simplex.pushBack(supportA, supportB);
        if (dot(supportPoint, direction) < 0) {
            // No collision
            return {false, simplex};
        }
        if(updateSimplex(simplex, direction)) {
            // Collision
            return {true, simplex};
        }
    }
    return {false, simplex};
}

CollisionSystem::ContactInfo CollisionSystem::distanceGJK(const SupportFunction& functionA,
        const SupportFunction& functionB, CollisionSystem::MinkowskyPolygon simplex) {
    assert(simplex.size() == 2 or simplex.size() == 3);

    // Special case: if the simplex contains only the closest point already
    if (norm(simplex.getDifference(0) - simplex.getDifference(1)) < eps) {
        return ContactInfo(simplex.getPointA(0), simplex.getPointB(0));
    }

    Vector2d closest;
    if (simplex.size() == 3) {
        // If we finished GJK collision with a 3-point simplex, remove a vertex
        closest = updateSimplexDistance(simplex);
    } else {
        closest = closestPoint(simplex.getDifference(0), simplex.getDifference(1), {0., 0.});
    }
    // The direction is always from the closest point towards the origin
    Vector2d direction = -closest;
    // The last added point on the simplex is the last support point
    Vector2d supportPoint = simplex.getDifference(1);

    const std::size_t maxIter{50};
    for (std::size_t t{0}; t < maxIter; ++t) {
        Vector2d supportA = functionA(direction);
        Vector2d supportB = functionB(-direction);
        Vector2d oldSupportPoint = supportPoint;
        supportPoint = supportA - supportB;
        // If we didn't get any closer to the origin
        if (std::abs(dot(oldSupportPoint, direction) - dot(supportPoint, direction)) < eps or t == maxIter - 1) {
            // We don't expect the projection of the origin to be on the
            // simplex, so we don't check the value of alpha
            return createContactInfo(simplex, 0, 1, false);
        }
        simplex.pushBack(supportA, supportB);
        direction = -updateSimplexDistance(simplex);
    }
    // Unreachable code
    assert(false);
    return ContactInfo();
}

CollisionSystem::ContactInfo CollisionSystem::EPA(const SupportFunction& functionA,
        const SupportFunction& functionB, CollisionSystem::MinkowskyPolygon polygon) {
    assert(polygon.size() == 3);

    std::size_t maxIter{100};
    for (std::size_t t{0}; t < maxIter; ++t) {
        // Find the polygon edge closest to the origin, and the associated
        // normal vector pointing away from the origin.
        // TODO we could cache the results of previous iteration, since at most
        // two edges are added to the polygon each iteration.
        double minDistance{std::numeric_limits<double>::max()};
        Vector2d minNormal;
        std::size_t minIndex;
        bool found{false};
        for (std::size_t i{0}; i < polygon.size(); ++i) {
            std::size_t j{(i + 1) % polygon.size()};
            Vector2d D_i{polygon.getDifference(i)}, D_j{polygon.getDifference(j)};
            Vector2d normal{perpendicular(D_j - D_i, D_i)};
            double distance{dot(normal, D_i)};
            normal /= norm(normal);
            if (distance < minDistance) {
                minDistance = distance;
                minNormal = normal;
                minIndex = i;
                found = true;
            }
        }
        assert(found);

        // Find the support point in the direction of the normal, and check
        // if this point is further
        Vector2d supportA{functionA(minNormal)};
        Vector2d supportB{functionB(-minNormal)};
        double supportDistance{dot(supportA - supportB, minNormal)};

        // If the point is no further, we found the closest edge.
        // We also stop here if we are at the end of the loop
        if (std::abs(supportDistance - minDistance) <= eps or t == maxIter - 1) {
            std::size_t j{(minIndex + 1) % polygon.size()};
            // Since the origin is in the simplex, it should be projected on the
            // edge, so we check the value of alpha
            return createContactInfo(polygon, minIndex, j, true);
        }
        // Otherwise, add the point to the polygon
        polygon.insert((minIndex + 1) % polygon.size(), supportA, supportB);
    }
    // Unreachable code
    assert(false);
    return ContactInfo();
}

CollisionSystem::ContactInfo CollisionSystem::createContactInfo(
        const CollisionSystem::MinkowskyPolygon& simplex,
        std::size_t i, std::size_t j, bool assertAlpha) {
    // Find the four points in A and B which correspond to the current
    // edge of the the polygon
    Vector2d A_i{simplex.getPointA(i)}, A_j{simplex.getPointA(j)};
    Vector2d B_i{simplex.getPointB(i)}, B_j{simplex.getPointB(j)};
    Vector2d D_i{A_i - B_i}, D_j{A_j - B_j};
    // Alpha is the barycentric coordinate between D_i and D_j
    // of the vector projection of the origin onto the line D_i S_J.
    // So if alpha = 0, then the origin maps to D_i, and if alpha = 1,
    // it maps to D_j
    double alpha{dot(D_j - D_i, -D_i) / norm2(D_j - D_i)};
    if (assertAlpha) {
        assert(0 <= alpha and alpha <= 1);
    }
    alpha = std::clamp(alpha, 0., 1.);

    return ContactInfo(A_i * (1 - alpha) + A_j * alpha, B_i * (1 - alpha) + B_j * alpha);
}

bool CollisionSystem::updateSimplex(CollisionSystem::MinkowskyPolygon& simplex, Vector2d& direction) {
    if (simplex.size() == 2) {
        return updateSimplexLine(simplex, direction);
    } else if (simplex.size() == 3) {
        return updateSimplexTriangle(simplex, direction);
    } else {
        assert(false);
    }
    return false;
}

bool CollisionSystem::updateSimplexLine(CollisionSystem::MinkowskyPolygon& simplex, Vector2d& direction) {
    Vector2d A{simplex.getDifference(1)}, B{simplex.getDifference(0)};
    // Vector going from the new point to the old point in the simplex
    Vector2d AB{B - A};
    // New direction is perpendicular to this vector (towards origin)
    direction = perpendicular(AB, -A);
    return false;
}

bool CollisionSystem::updateSimplexTriangle(CollisionSystem::MinkowskyPolygon& simplex, Vector2d& direction) {
    Vector2d A{simplex.getDifference(2)}, B{simplex.getDifference(1)}, C{simplex.getDifference(0)};
	Vector2d AC{C - A}, AB{B - A};
    // Check if we are outside the AC face. The perpendicular vector
    // goes away from AB, so that it points outwards of the triangle
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

Vector2d CollisionSystem::updateSimplexDistance(CollisionSystem::MinkowskyPolygon& simplex) {
    Vector2d closest0{closestPoint(simplex.getDifference(2), simplex.getDifference(0), {0., 0.})};
    Vector2d closest1{closestPoint(simplex.getDifference(2), simplex.getDifference(1), {0., 0.})};
    if (norm2(closest0) < norm2(closest1)) {
        simplex.erase(1);
        return closest0;
    } else {
        simplex.erase(0);
        return closest1;
    }
}
