#pragma once

struct vec3f {
	float x, y, z;

	vec3f() : x(0.0f), y(0.0f), z(0.0f) { }
	vec3f(float a, float b, float c) : x(a), y(b), z(c) { }

	vec3f &operator*=(float other) {
		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	vec3f &operator*(float other) {
		*this *= other;
		return *this;
	}
};