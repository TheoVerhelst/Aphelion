#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <vector>
#include <utility>
#include <vector.hpp>
#include <Body.hpp>

// Information about the collision between two bodies. It is defined as a point
// from the body A and a face from the body B (defined by two points). The
// boolean argument indicates which Body instance is the body A.
struct CollisionResponse {
	Vector2d A, B_1, B_2;
	bool AisThis;
};

// Collision detection and response between arbitrary bodies.
void collideBodies(Body& bodyA, Body& bodyB);

// The Gilbert–Johnson–Keerthi algorithm detects collisions between two
// bodies, and returns the point of contact between them.
std::optional<CollisionResponse> GJK(const Body& bodyA, const Body& bodyB);

// In the special case of two circle, the collision detection and response is
// much simpler, so we do both at once here.
void collideCircles(CircleBody& bodyA, CircleBody& bodyB);

namespace impl {

bool nearestSimplex(std::vector<Vector2d>& simplex, Vector2d& direction,
        std::vector<std::pair<Vector2d, Vector2d>>& simplexPoints);

bool simplexLine(std::vector<Vector2d>& simplex, Vector2d& direction);

bool simplexTriangle(std::vector<Vector2d>& simplex, Vector2d& direction,
        std::vector<std::pair<Vector2d, Vector2d>>& simplexPoints);

} // namespace impl

#endif // COLLISIONS_HPP
