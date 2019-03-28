#pragma once

struct vec3 {
	float x, y, z;

public:
	vec3();
	vec3(float a, float b, float c);
};

struct vec4 {
	float x, y, z, w;

public:
	vec4();
	vec4(float a, float b, float c, float d);
};

struct mat4 {
	float m[16];

public:
	mat4();
	explicit mat4(float *values);
	mat4(float *values, size_t n);
	mat4(
		float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44
	);

	float get_elem(int row, int col) const;

	void fill(float value);

	vec4 &operator[](size_t n);

	mat4 &operator+=(const mat4 &rhs);
	mat4 &operator-=(const mat4 &rhs);
	mat4 &operator*=(const mat4 &rhs);
	mat4 &operator*=(const float factor);

	static mat4 identity();
	static mat4 scale(float factor);
	static mat4 translate(const vec3 &vector);
};
