#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <vector>
#include <array>
#include <vector.hpp>


class ConvexPolygon {
public:
	ConvexPolygon(const std::vector<Vector2f>& vertices);
    Vector2f supportFunction(const Vector2f& direction) const;
	float momentOfInertia(float density, const Vector2f& axis) const;
	std::pair<float, Vector2f> areaAndCenterOfMass() const;
    // Checks if a convex polygon contains a given point by checking if it is on
    // the right side of each edge, going clockwise.
    bool contains(const Vector2f& P) const;

private:
	std::vector<Vector2f> _vertices;
};

// Checks if a rectangular box contains the point P. The box may not be aligned
// with the axes. The corner order in the array is clockwise, starting from the
// top left corner.
bool boxContains(const std::array<Vector2f, 4>& box, const Vector2f& P);

std::vector<std::vector<std::size_t>> earClipping(const std::vector<Vector2f>& vertices);

std::vector<std::vector<std::size_t>> HertelMehlhorn(
        const std::vector<Vector2f>& vertices,
        std::vector<std::vector<std::size_t>> triangulation);

#endif // POLYGON_HPP
