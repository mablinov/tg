#include <sstream>
#include <iostream>

#include "Shader.h"

#if 0
Shader::Shader(std::string source, GLenum type) :
	m_source(source),
	m_type(type),
	m_id(0),
	m_valid(false),
	m_state(UNINITIALIZED)
{
	assert(source.length() != 0);
	assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

	auto id = glCreateShader(type);
	if (!id) {
		throw std::runtime_error("glCreateShader returned 0");
	}

	auto string = source.c_str();
	glShaderSource(id, 1, &string, NULL);

	auto err = glGetError();
	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Supplied shader id was not generated by OpenGL");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error("Supplied shader id does not represent a shader object");
		break;
	default:
		throw std::runtime_error("Unknown exception after glShaderSource");
		break;
	}

	glCompileShader(id);
	err = glGetError();

	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Supplied shader id was not generated by OpenGL");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error("Supplied shader id does not represent a shader object");
		break;
	default:
		throw std::runtime_error("Unknown exception after glShaderSource");
		break;
	}

	m_id = id;
	m_valid = true;
}
#endif

Shader::Shader() :
	m_source(""),
	m_type(GL_NONE),
	m_id(0),
	m_state(UNINITIALIZED)
{ }

Shader::Shader(std::string source, GLenum type) :
	m_source(""),
	m_type(GL_NONE),
	m_id(0),
	m_state(UNINITIALIZED)
{
	create(type);
	load_source(source);
	compile();
}

void Shader::create(GLenum type)
{
	assert(m_state == UNINITIALIZED);
	assert(is_shader_type(type));

	auto id = glCreateShader(type);
	if (!id) {
		throw std::runtime_error("glCreateShader returned 0");
	}

	m_type = type;
	m_id = id;
	m_state = CREATED;
}

void Shader::load_source(std::string source)
{
	assert(m_state == CREATED);
	assert(source.length() != 0);

	const char* string = source.c_str();
	glShaderSource(m_id, 1, &string, NULL);

	GLenum err = glGetError();
	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Supplied shader id was not generated by OpenGL");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error("Supplied shader id does not represent a shader object");
		break;
	default:
		throw std::runtime_error("Unknown exception after glShaderSource");
		break;
	}

	m_source = source;
	m_state = SOURCED;
}

void Shader::compile()
{
	assert(m_state == SOURCED);

	glCompileShader(m_id);
	GLenum err = glGetError();

	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Supplied shader id was not generated by OpenGL");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error("Supplied shader id does not represent a shader object");
		break;
	default:
		throw std::runtime_error("Unknown exception after glShaderSource");
		break;
	}

	GLint compile_status = get_shader_param(GL_COMPILE_STATUS);
	if (compile_status != GL_TRUE) {
		std::cout << "Failed to compile:\n" << log() << "\n";
	}

	assert(compile_status == GL_TRUE);

	m_state = COMPILED;
}

std::string Shader::log() const
{
	GLint log_size = get_shader_param(GL_INFO_LOG_LENGTH);
	std::string buffer((size_t)log_size, '\0');

	glGetShaderInfoLog(m_id, max_log_length, NULL, (GLchar*)buffer.c_str());
	assert(glGetError() == GL_NONE);

	return buffer;
}

bool Shader::is_shader_type(GLenum type)
{
	return
		(type == GL_FRAGMENT_SHADER)
		|| (type == GL_VERTEX_SHADER)
		|| (type == GL_GEOMETRY_SHADER)
		|| (type == GL_COMPUTE_SHADER);
}

bool Shader::is_shader_param(GLenum param)
{
	return (param == GL_SHADER_TYPE)
		|| (param == GL_DELETE_STATUS)
		|| (param == GL_COMPILE_STATUS)
		|| (param == GL_INFO_LOG_LENGTH)
		|| (param == GL_SHADER_SOURCE_LENGTH);
}

std::string Shader::get_shader_type_str(GLenum type)
{
	assert(is_shader_type(type));

	switch (type) {
	case GL_VERTEX_SHADER: return std::string("GL_VERTEX_SHADER");
	case GL_FRAGMENT_SHADER: return std::string("GL_FRAGMENT_SHADER");
	case GL_GEOMETRY_SHADER: return std::string("GL_GEOMETRY_SHADER");
	default: return std::string("UNKNOWN_SHADER_TYPE");
	}
}

GLint Shader::get_shader_param(GLenum param) const
{
	assert(is_shader_param(param));

	GLint value = 0;
	glGetShaderiv(m_id, param, &value);

	assert(glGetError() == GL_NONE);

	return value;
}

bool Shader::ready_to_link() const
{
	return 
		m_state == COMPILED
		&& (get_shader_param(GL_DELETE_STATUS) == GL_FALSE)
		&& (get_shader_param(GL_COMPILE_STATUS) == GL_TRUE);
}

void Shader::print_debug_info() const
{
	static int param_count = 5;
	static struct {
		GLenum p;
		const char* str;
		int type;
	} prog_params[] = {
		{GL_SHADER_TYPE, "GL_SHADER_TYPE", GL_SHADER},
		{GL_DELETE_STATUS, "GL_DELETE_STATUS", GL_BOOL},
		{GL_COMPILE_STATUS, "GL_COMPILE_STATUS", GL_BOOL},
		{GL_INFO_LOG_LENGTH, "GL_INFO_LOG_LENGTH", GL_INT},
		{GL_SHADER_SOURCE_LENGTH, "GL_SHADER_SOURCE_LENGTH", GL_INT},
	};

	std::cout << "========| Shader::print_debug_info() |========\n";

	std::cout << "Internal state variables:\n";
	std::cout << "\tm_type: " << get_shader_type_str(m_type) << "\n";
	std::cout << "\tm_id: " << (int)m_id << "\n";
	std::cout << "\tm_state: ";
	switch (m_state) {
	case UNINITIALIZED: std::cout << "UNINITIALIZED\n"; break;
	case CREATED: std::cout << "CREATED\n"; break;
	case SOURCED: std::cout << "SOURCED\n"; break;
	case COMPILED: std::cout << "COMPILED\n"; break;
	}

	for (int i = 0; i < param_count; ++i) {
		GLint value;

		// First, get the value.
		glGetShaderiv(m_id, prog_params[i].p, &value);
		GLenum err = glGetError();
		assert(err == GL_NONE);

		// Stringify according to its type (as specified in the table)
		std::stringstream ss;

		switch (prog_params[i].type) {
		case GL_BOOL:
			ss << (value ? "GL_TRUE" : "GL_FALSE");
			break;
		case GL_INT:
			ss << (int)value;
			break;
		case GL_SHADER:
			ss << get_shader_type_str(value);
			break;
		default:
			assert(false);
			break;
		}

		std::cout << prog_params[i].str << ": " << ss.str() << "\n";
	}

	std::cout << "log:\n";
	std::cout << log();

	std::cout << "========| ***END*** Shader::print_debug_info() |========\n";
}
