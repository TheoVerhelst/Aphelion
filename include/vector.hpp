#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include <iostream>
#include <array>
#include <vector>
#include <SFML/System/Vector2.hpp>

typedef sf::Vector2<double> Vector2d;
typedef sf::Vector2<float> Vector2f;
typedef sf::Vector2<int> Vector2i;
typedef sf::Vector2<unsigned int> Vector2u;

const float pi{static_cast<float>(std::acos(-1))};

template <std::floating_point T>
constexpr T degToRad(T v) {
	return v * std::acos(static_cast<T>(-1)) / static_cast<T>(180);
}

template <std::floating_point T>
constexpr T radToDeg(T v) {
	return v * static_cast<T>(180) / std::acos(static_cast<T>(-1));
}

template <typename T>
constexpr T dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.x + a.y * b.y;
}

template <std::floating_point T>
T norm(const sf::Vector2<T>& vector) {
	return std::hypot(vector.x, vector.y);
}

template <typename T>
constexpr T norm2(const sf::Vector2<T>& vector) {
	return dot(vector, vector);
}

template <typename T>
T angle(const sf::Vector2<T>& vector) {
	return std::atan2(vector.y, vector.x);
}

template <typename T>
T angle(const sf::Vector2<T>& A, const sf::Vector2<T>& B, const sf::Vector2<T>& C) {
	const T res{angle(A - B) - angle(C - B)};
	return res < 0 ? res + static_cast<T>(2 * pi) : res;
}

// Vertical component of the cross product of two 2D vectors. We return a scalar
// instead of a Vector3 because we need only its norm.
template <typename T>
constexpr T cross(const sf::Vector2<T>& a, const sf::Vector2<T>& b) {
	return a.x * b.y - a.y * b.x;
}

// Returns a vector perpendicular to v, with the specified winding. Winding left
// means that the resulting vector will go to the left of v. For example, the
// perpendicular of (1, 0) is (0, 1) when winding left, and (0, -1) when winding
// right. This is assuming the x-axis going left and the y-axis going down.
template <typename T>
constexpr sf::Vector2<T> perpendicular(const sf::Vector2<T>& v, bool windLeft) {
	return windLeft ? sf::Vector2<T>(v.y, -v.x) : sf::Vector2<T>(-v.y, v.x);
}

// Returns a vector perpendicular to v, such that it has a positive dot product
// with the vector d.
template <typename T>
constexpr sf::Vector2<T> perpendicular(const sf::Vector2<T>& v, const sf::Vector2<T>& d) {
	// We could use the formula (v x d) x v, but this requires three
	// multiplications, which is not ideal for small vectors. We have a branch
	// but no number stability issue here.
	Vector2f res{v.y, -v.x};
	if (dot(res, d) < 0) {
		return -res;
	} else {
		return res;
	}
}

template <std::floating_point T>
sf::Vector2<T> rotate(const sf::Vector2<T>& v, T angle) {
	return {v.x * std::cos(angle) - v.y * std::sin(angle), v.x * std::sin(angle) + v.y * std::cos(angle)};
}

// Compute the intersection P between the line AB and CD. The result is the
// barycentric coordinate u, v of P on the line AB and CD. It is such that
// P = A + u * (B - A) = C + v * (D - C). So when 0 < u < 1, P is located
// between A and B, and when 0 < v < 1, P is located between C and D
template <std::floating_point T>
constexpr std::pair<T, T> intersection(const sf::Vector2<T>& A, const sf::Vector2<T>& B,
		const sf::Vector2<T>& C, const sf::Vector2<T>& D) {
	const sf::Vector2<T> S{B - A};
	const sf::Vector2<T> R{D - C};
	const sf::Vector2<T> CA{A - C};
	const T RxS{cross(R, S)};
	return {cross(CA, R) / RxS, cross(CA, S) / RxS};
}


// Closest point to P on the segment AB.
template <std::floating_point T>
sf::Vector2<T> closestPoint(const sf::Vector2<T>& A, const sf::Vector2<T>& B,
		const sf::Vector2<T>& P) {
	const Vector2f AB{B - A};
	const T c{dot(AB, P - A) / norm2(AB)};
	if (c < 0) {
		return A;
	} else if (c > 1) {
		return B;
	} else {
		return A + c * AB;
	}
}

template <std::floating_point T>
bool convexContains(const std::vector<sf::Vector2<T>>& convex, const sf::Vector2<T>& P) {
	for (std::size_t i{0}; i < convex.size(); ++i) {
		std::size_t j{(i + 1) % convex.size()};
		std::size_t k{(i + 2) % convex.size()};
		if (cross(convex[j] - convex[i], P - convex[i]) * cross(convex[j] - convex[i], convex[k] - convex[i]) < 0) {
			return false;
		}
	}
	return true;
}

// Checks if a rectangular box contains the point P. The box may not be aligned
// with the axes. The corner order in the array is (up left) - (up right) -
// (down right) (down left).
template <std::floating_point T>
bool boxContains(const std::array<sf::Vector2<T>, 4>& box, const sf::Vector2<T>& P) {
	//  A -------- B
	//  |   P      |
	//  D -------- C
	const sf::Vector2<T> AB{box[1] - box[0]};
	const sf::Vector2<T> AP{P      - box[0]};
	const sf::Vector2<T> BP{P      - box[1]};
	const sf::Vector2<T> AD{box[3] - box[0]};
	const sf::Vector2<T> DP{P      - box[3]};
	return dot(AB, AP) > 0 and dot(AB, BP) < 0 and dot(AD, AP) > 0 and dot(AD, DP) < 0;
}


template <std::floating_point T>
sf::Vector2<T> clampVector(sf::Vector2<T> v, const sf::Vector2<T>& min, const sf::Vector2<T>& max) {
	const T aspectRatio{v.x / v.y};
    if (v.x > max.x) {
        v = {max.x, max.x / aspectRatio};
    }
    if (v.y > max.y) {
        v = {max.y * aspectRatio, min.y};
    }
    if (v.x < min.x) {
        v = {min.x, min.x / aspectRatio};
    }
    if (v.y < min.y) {
        v = {min.y * aspectRatio, min.y};
    }
    return v;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
	return os << "(" << v.x << ", " << v.y << ")";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vector) {
	os << "[";
	for (std::size_t i{0}; i < vector.size(); ++i) {
		os << vector[i] << (i < vector.size() - 1 ? ", " : "");
	}
	return os << "]";
}

#endif // VECTOR_HPP
