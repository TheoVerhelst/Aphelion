#include <components.hpp>

Vector2d Body::localToWorld(const Vector2d& point) const {
    return rotate(point, rotation) + position;
}

Vector2d Body::worldToLocal(const Vector2d& point) const {
    return rotate(point - position, -rotation);
}
