#pragma once

struct vec4f {
	float x, y, z, w;

	vec4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
	vec4f(float a, float b, float c, float d) : x(a), y(b), z(c), w(d) { }
};

using rgbaf = vec4f;
