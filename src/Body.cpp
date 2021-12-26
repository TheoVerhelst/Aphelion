#include <Body.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>

void CircleBody::collide(Body& other) {
    other.collide(*this);
}

void CircleBody::collide(CircleBody& other) {
    CircleBody& a = *this;
    CircleBody& b = other;
    const Vector2d diff_x = b.position - a.position;
    const double dist{norm(diff_x)};
    const double overlap{a.radius + b.radius - dist};

    // Collision
    if (overlap > 0) {
        // Distance squared
        const double dist_2{dot(diff_x, diff_x)};
        const Vector2d v_a = a.velocity;
        const Vector2d v_b = b.velocity;
        const double m_a = a.mass;
        const double m_b = b.mass;
        // Based on stackoverflow.com/questions/35211114/2d-elastic-ball-collision-physics
        const Vector2d addedVel = 2 * dot(v_a - v_b, diff_x) * diff_x / ((m_a + m_b) * dist_2);
        a.velocity = v_a - addedVel * m_b;
        b.velocity = v_b + addedVel * m_a;

        // Move the bodies so that they just touch and don't overlap.
        // The displacement is proportional to the mass of the other body.
        a.position -= m_b * overlap * diff_x / (dist * (m_a + m_b));
        b.position += m_a * overlap * diff_x / (dist * (m_a + m_b));
    }
}

void CircleBody::collide(PolygonBody& other) {
    // TODO GJK as well
}

std::shared_ptr<sf::Shape> CircleBody::createShape() const {
    std::shared_ptr<sf::CircleShape> circle{new sf::CircleShape(static_cast<float>(radius))};
    circle->setOrigin(radius, radius);
    circle->setFillColor(color);
    return circle;
}

Vector2d CircleBody::support(Vector2d direction) const {
    return position + direction * radius / norm(direction);
}

void PolygonBody::collide(Body& other) {
    other.collide(*this);
}

void PolygonBody::collide(CircleBody& other) {
    other.collide(*this);
}

void PolygonBody::collide(PolygonBody& other) {
    // GJK algorithm, see https://blog.winter.dev/2020/gjk-algorithm/
    Vector2d direction{1, 0};
    Vector2d a{support(direction) - other.support(-direction)};
    std::vector<Vector2d> simplex{a};
    direction = -a;
    while (true) {
        a = support(direction) - other.support(-direction);
        if (dot(a, direction) < 0) {
            // No collision
            return;
        }
        simplex.push_back(a);
        if(nearestSimplex(simplex, direction)) {
            std::cout << "Collision" << std::endl;
            return;
        }
    }
}

std::shared_ptr<sf::Shape> PolygonBody::createShape() const {
    std::shared_ptr<sf::ConvexShape> polygon{new sf::ConvexShape(vertices.size())};
    for(std::size_t i{0}; i < vertices.size(); ++i) {
        polygon->setPoint(i, static_cast<Vector2f>(vertices[i]));
    }
    polygon->setFillColor(color);
    return polygon;
}

Vector2d PolygonBody::support(Vector2d direction) const {
    std::vector<double> products(vertices.size());
    // Compute all dot products between vertices and direction
    std::transform(
        vertices.begin(),
        vertices.end(),
        products.begin(),
        std::bind(dot<double>, std::placeholders::_1, direction)
    );
    // Find the largest one
    auto largest{std::distance(
        products.begin(),
        std::max_element(products.begin(), products.end())
    )};
    return vertices[largest];
}

bool PolygonBody::nearestSimplex(std::vector<Vector2d>& simplex, Vector2d& direction) {
    if (simplex.size() == 2) {
        return simplexLine(simplex, direction);
    } else if (simplex.size() == 3) {
        return simplexTriangle(simplex, direction);
    } else {
        throw std::runtime_error("Invalid simplex size");
    }
    return false;
}

bool PolygonBody::simplexLine(std::vector<Vector2d>& simplex, Vector2d& direction) {
    Vector2d a{simplex[0]}, b{simplex[1]};
    // Vector going from the new point to the old point in the simplex
    Vector2d ba{a - b};
    // If the origin is on the same side as this vector
    if (dot(ba, -b) > 0) {
        // New direction is perpendicular to this vector (towards origin)
        direction = perpendicular(ba, -b);
    } else {
        simplex = {b};
        direction = -b;
    }
    return false;
}

bool PolygonBody::simplexTriangle(std::vector<Vector2d>& simplex, Vector2d& direction) {
    Vector2d a{simplex[0]}, b{simplex[1]}, c{simplex[2]};
	Vector2d ca{a - c}, cb{b - c};
    // Check if we are outside the AC face. We go away from cb to stay outside
    // the simplex.
    Vector2d ca_perp{perpendicular(ca, -cb)};
    // If the origin is on this side
    if (dot(ca_perp, -c) > 0) {
        if (dot(ca, -c) > 0) {
            simplex = {a, c};
            direction = ca_perp;
        } else {
            // Not sure we could arrive here
            throw std::runtime_error("Not possible ????");
        }
        return false;
    }
    Vector2d cb_perp{perpendicular(cb, -ca)};
    if (dot(cb_perp, -c) > 0) {
        simplex = {b, c};
        direction = cb_perp;
        return false;
    } else {
        // Must be inside the simplex
        return true;
    }
}
