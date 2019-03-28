#pragma once

#include <string>
#include <assert.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

class Shader {
	std::string m_source;
	GLenum m_type;
	GLuint m_id;

	enum state {
		UNINITIALIZED = 0,
		CREATED,
		SOURCED,
		COMPILED
	} m_state;

public:
	Shader();
	Shader(std::string source, GLenum type);

	void create(GLenum type);
	void load_source(std::string source);
	void compile();

	// Getters
	const std::string& source() const { return m_source; }
	GLuint type() const { return m_type; }
	GLuint id() const { return m_id; }

	// Utilities
	std::string log() const;
	GLint get_shader_param(GLenum param) const;
	bool ready_to_link() const;
	void print_debug_info() const;

	static const GLsizei max_log_length = 4000;

private:
	// Helper functions
	static bool is_shader_type(GLenum type);
	static bool is_shader_param(GLenum param);
	static std::string get_shader_type_str(GLenum type);
};