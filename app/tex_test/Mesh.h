#pragma once

#include "WavefrontObj.h"

#include "vec3f.h"
#include "vec2f.h"

#include "GL/glew.h"
#include "GL/glut.h"

#include <cstdint>
#include <vector>

struct Mesh {
	struct index_tri {
		size_t p1, p2, p3;

		index_tri();
		index_tri(size_t i1, size_t i2, size_t i3);
	};

	struct Unpacked {
		std::vector<vec3f> vertex;
		std::vector<vec2f> uv;
		std::vector<vec3f> normal;
	};

	std::vector<vec3f> vertex;
	std::vector<vec2f> uv;
	std::vector<vec3f> normal;

	std::vector<index_tri> vertex_tri;
	std::vector<index_tri> uv_tri;
	std::vector<index_tri> normal_tri;

public:
	explicit Mesh(WavefrontObj &obj);

	Unpacked unpack_to(GLenum mode);
	Unpacked unpack_to_triangles();
};