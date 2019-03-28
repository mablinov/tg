#pragma once

#include "GL/glew.h"
#include "GL/glut.h"

#include "vec4f.h"
#include "TGAImage.h"

#include "Shader.h"
#include "Program.h"

class App {
	struct gl_buffer {
		GLuint vertex;
		GLuint normal;
		GLuint uv;
	} buffer;

	GLuint tex;

	Shader *vs, *fs;
	Program *p;

	size_t triangle_count;

public:
	App();
	void init();
	size_t get_triangle_count() const;

private:
	std::vector<vec4f> conv_tga_to_gltexture(const TGAImage &image) const;

	void init_array();
	void init_tex();
	void init_program();
};
