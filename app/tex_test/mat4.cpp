#include "mat4.h"

mat4::mat4() {
	for (size_t i = 0; i < 16; ++i) {
		m[i] = 0.0f;
	}
}

mat4::mat4(float * values)
{
	for (size_t i = 0; i < 16; ++i) {
		m[i] = values[i];
	}
}

mat4::mat4(float * values, size_t n)
{
	for(size_t i = 0; i < n; ++i) {
		m[i] = values[i];
	}
}

mat4::mat4(
	float m11, float m12, float m13, float m14,
	float m21, float m22, float m23, float m24,
	float m31, float m32, float m33, float m34,
	float m41, float m42, float m43, float m44
)
{
	m[0] = m11; m[4] = m12; m[8] = m13; m[12] = m14;
	m[1] = m21; m[5] = m22; m[9] = m23; m[13] = m24;
	m[2] = m31; m[6] = m32; m[10] = m33; m[14] = m34;
	m[3] = m41; m[7] = m42; m[11] = m43; m[15] = m44;
}

float mat4::get_elem(int row, int col) const
{
	return m[col * 4 + row];
}

void mat4::fill(float value)
{
	for (int i = 0; i < 16; ++i) {
		m[i] = value;
	}
}

vec4 & mat4::operator[](size_t n)
{
	vec4 *col = (vec4 *)&m[n * 4];
	vec4 &ref = *col;

	return ref;
}

mat4 & mat4::operator+=(const mat4 & rhs)
{
	for (int i = 0; i < 16; ++i) {
		m[i] += rhs.m[i];
	}

	return *this;
}

mat4 & mat4::operator-=(const mat4 & rhs)
{
	for (int i = 0; i < 16; ++i) {
		m[i] -= rhs.m[i];
	}

	return *this;
}

mat4 & mat4::operator*=(const mat4 & rhs)
{
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			m[col * 4 + row] =
				m[0 * 4 + row] * rhs.m[col * 4 + 0]
				+ m[1 * 4 + row] * rhs.m[col * 4 + 1]
				+ m[2 * 4 + row] * rhs.m[col * 4 + 2]
				+ m[3 * 4 + row] * rhs.m[col * 4 + 3];
		}
	}

	return *this;
}

mat4 & mat4::operator*=(const float factor)
{
	for (int i = 0; i < 16; ++i) {
		m[i] *= factor;
	}

	return *this;
}

mat4 mat4::identity()
{
	mat4 m;

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			m.m[col * 4 + row] = (col == row ? 1.0f : 0.0f);
		}
	}

	return m;
}

mat4 mat4::scale(float factor)
{
	mat4 m;

	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			m.m[col * 4 + row] = (col == row ? factor : 0.0f);
		}
	}

	return m;
}

mat4 mat4::translate(const vec3 & vector)
{
	mat4 m = mat4::identity();
	m.m[12] = vector.x;
	m.m[13] = vector.y;
	m.m[14] = vector.z;

	return m;
}

vec3::vec3() : x(0.0f), y(0.0f), z(0.0f)
{
}

vec3::vec3(float a, float b, float c)
	: x(a), y(b), z(c)
{
}

vec4::vec4()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

vec4::vec4(float a, float b, float c, float d)
	: x(a), y(b), z(c), w(d)
{
}
