#ifndef COLLISIONS_HPP
#define COLLISIONS_HPP

#include <vector>
#include <optional>
#include <utility>
#include <vector.hpp>
#include <Body.hpp>

// Collision detection and response between arbitrary bodies.
void collideBodies(Body& bodyA, Body& bodyB);

// In the special case of two circle, the collision detection and response is
// much simpler, so we do both at once here.
void collideCircles(CircleBody& bodyA, CircleBody& bodyB);

namespace impl {

class MinkowskyPolygon {
public:
	void pushBack(const Vector2d& A, const Vector2d& B);
	void insert(std::size_t index, const Vector2d& A, const Vector2d& B);
	void erase(std::size_t index);
	Vector2d getDifference(std::size_t index) const;
	Vector2d getPointA(std::size_t index) const;
	Vector2d getPointB(std::size_t index) const;
	std::size_t size() const;

private:
	std::vector<Vector2d> _pointsA, _pointsB;
};

// Information about the contact points between two bodies.
struct ContactInfo {
	// Point in world coordinates where body A and body B touch. There are two
	// different points because the bodies can overlap.
	Vector2d C_A, C_B;
	// Collision normal vector, pointing from A to B. If B is shifted by this
	// vector, the bodies will just touch but not overlap anymore.
	Vector2d normal;
};

// The Gilbert–Johnson–Keerthi algorithm detects collisions between two
// bodies, and returns the simplex in the Minkowsky difference that encloses
// the origin.
std::optional<MinkowskyPolygon> GJK(const Body& bodyA, const Body& bodyB);

// The Expanding Polytope Algorithm computes the collision vector, which is the
// minimum vector that would separate the shapes again. It takes as input the
// simplex enclosing the origin computed by GJK.
ContactInfo EPA(const Body& bodyA, const Body& bodyB, MinkowskyPolygon polygon);

bool nearestSimplex(MinkowskyPolygon& simplex, Vector2d& direction);

bool simplexLine(MinkowskyPolygon& simplex, Vector2d& direction);

bool simplexTriangle(MinkowskyPolygon& simplex, Vector2d& direction);

} // namespace impl

#endif // COLLISIONS_HPP
