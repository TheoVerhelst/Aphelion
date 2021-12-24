#include <Body.hpp>
#include <iostream>

void CircleBody::collide(Body& other) {
    other.collide(*this);
}

void CircleBody::collide(CircleBody& other) {
    CircleBody& a = *this;
    CircleBody& b = other;
    const Vector2d diff_x = a.position - b.position;
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
    // TODO
}

void PolygonBody::collide(Body& other) {
    other.collide(*this);
}

void PolygonBody::collide(CircleBody& other) {
    other.collide(*this);
}

void PolygonBody::collide(PolygonBody& other) {
    // TODO: CJK algorithm
}
