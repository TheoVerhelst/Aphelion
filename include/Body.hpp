#ifndef BODY_HPP
#define BODY_HPP

#include <SFML/Graphics.hpp>
#include <helpers.hpp>

struct Body {
	double mass;
	double radius;
	Vector2d position;
	Vector2d velocity;
};

#endif // BODY_HPP
