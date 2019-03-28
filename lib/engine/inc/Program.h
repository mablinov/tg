#pragma once

#include <exception>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shader.h"

class Program {
	GLuint m_program_id;
	enum state_t { UNINITIALIZED, CREATED, LINKED, USED } m_state;

	void init();
public:

	Program();
	Program(const Shader& s);
	Program(const Shader& vs, const Shader& fs);

	//Program(const Shader& vs, const Shader& fs, const Shader& gs);
	//Program(const Shader& vs, const Shader& fs, const Shader& gs, const Shader& ts);

	void add_shader(const Shader& s) const;

	void link();
	void use();

	std::string log() const;
	void print_debug_info() const;

	GLuint id() const { return m_program_id; }

	// Utilities
	GLint get_program_param(GLenum param) const;
	std::string get_attribute_info() const;

private:
	// Helper functions
	static bool is_program_param(GLenum param);
};
