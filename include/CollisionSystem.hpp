#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

#include <vector>
#include <cstddef>
#include <optional>
#include <vector.hpp>
#include <Scene.hpp>
#include <components.hpp>

class CollisionSystem {
public:
	CollisionSystem(Scene& scene);
	void update();

private:
	Scene& _scene;

	// List of 2D points formed by the difference of two shapes. This class
	// makes it easy to store and pass around the difference and the original
	// points together.
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

	void collideBodies(const Collider& colliderA, const Collider& colliderB, Body& bodyA, Body& bodyB);

	// In the special case of two circle, the collision detection and response is
	// much simpler, so we do both at once here.
	void collideCircles(const CircleBody& circleA, const CircleBody& circleB, Body& bodyA, Body& bodyB);

	// Collision response between arbitrary bodies.
	void collisionResponse(Body& bodyA, Body& bodyB, const ContactInfo& contactInfo);

	// The Gilbert–Johnson–Keerthi algorithm detects collisions between two
	// bodies, and returns the simplex in the Minkowsky difference that encloses
	// the origin.
	std::optional<MinkowskyPolygon> GJK(const Collider& colliderA, const Collider& colliderB);

	// The Expanding Polytope Algorithm computes the collision vector, which is the
	// minimum vector that would separate the shapes again. It takes as input the
	// simplex enclosing the origin computed by GJK.
	ContactInfo EPA(const Collider& colliderA, const Collider& colliderB, MinkowskyPolygon polygon);

	bool nearestSimplex(MinkowskyPolygon& simplex, Vector2d& direction);

	bool simplexLine(MinkowskyPolygon& simplex, Vector2d& direction);

	bool simplexTriangle(MinkowskyPolygon& simplex, Vector2d& direction);
};

#endif // COLLISIONSYSTEM_HPP
