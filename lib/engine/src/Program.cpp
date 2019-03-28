#include <sstream>
#include <vector>
#include <iostream>
#include <map>

#include "Program.h"

Program::Program() :
	m_program_id(0),
	m_state(UNINITIALIZED)
{
	init();
}

Program::Program(const Shader& s) :
	m_program_id(0),
	m_state(UNINITIALIZED)
{
	init();
	add_shader(s);

	link();
	use();
}

Program::Program(const Shader& vs, const Shader& fs) :
	m_program_id(0),
	m_state(UNINITIALIZED)
{
	init();
	add_shader(vs);
	add_shader(fs);

	link();
}

void Program::init()
{
	assert(m_program_id == 0);
	assert(m_state == UNINITIALIZED);

	GLuint id = glCreateProgram();
	assert(id);

	m_program_id = id;
	m_state = CREATED;
}

void Program::add_shader(const Shader& s) const {
	assert(m_program_id);
	assert(m_state == CREATED);
	assert(s.ready_to_link());

	glAttachShader(m_program_id, s.id());
	GLenum err = glGetError();

	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Shader or program object are invalid");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error("Could not attach shader object to program object");
		break;
	}
}

void Program::link() {
	assert(m_program_id);
	assert(m_state == CREATED);
	glLinkProgram(m_program_id);
	assert(glGetError() == GL_NONE);
	
	m_state = LINKED;
}

void Program::use() {
	assert(m_program_id);
	assert(m_state == LINKED);
	glUseProgram(m_program_id);

	GLenum err = glGetError();
	switch (err) {
	case GL_NONE:
		break;
	case GL_INVALID_VALUE:
		throw std::runtime_error("Failed to use program: invalid program object");
		break;
	case GL_INVALID_OPERATION:
		throw std::runtime_error(
			"Failed to use program: not a program object, "
			"or could not make part of state, "
			"or transform feedback mode is active");
		break;
	default:
		break;
	}

	m_state = USED;
}

std::string Program::log() const
{
	assert(m_state != UNINITIALIZED);

	GLint info_log_length = get_program_param(GL_INFO_LOG_LENGTH);
	std::string buffer(info_log_length, '\0');

	glGetProgramInfoLog(m_program_id, info_log_length, NULL, (GLchar*)buffer.c_str());
	assert(glGetError() == GL_NONE);

	return buffer;
}

void Program::print_debug_info() const
{
	static int param_count = 9;
	static struct {
		GLenum p;
		const char* str;
		int type;
	} prog_params[] = {
		{GL_DELETE_STATUS, "GL_DELETE_STATUS", GL_BOOL},
		{GL_LINK_STATUS, "GL_LINK_STATUS", GL_BOOL},
		{GL_VALIDATE_STATUS, "GL_VALIDATE_STATUS", GL_BOOL},
		{GL_INFO_LOG_LENGTH, "GL_INFO_LOG_LENGTH", GL_INT},
		{GL_ATTACHED_SHADERS, "GL_ATTACHED_SHADERS", GL_INT},
		{GL_ACTIVE_ATTRIBUTES, "GL_ACTIVE_ATTRIBUTES", GL_INT},
		{GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH", GL_INT},
		{GL_ACTIVE_UNIFORMS, "GL_ACTIVE_UNIFORMS", GL_INT},
		{GL_ACTIVE_UNIFORM_MAX_LENGTH, "GL_ACTIVE_UNIFORM_MAX_LENGTH", GL_INT}
	};
	
	std::cout << "========| Program::print_debug_info() |========\n";

	std::cout << "Internal state variables:\n";
	std::cout << "\tm_program_id: " << (int)m_program_id << "\n";
	std::cout << "\tm_state: ";
	switch (m_state) {
	case UNINITIALIZED: std::cout << "UNINITIALIZED\n"; break;
	case CREATED: std::cout << "CREATED\n"; break;
	case LINKED: std::cout << "LINKED\n"; break;
	case USED: std::cout << "USED\n"; break;
	}

	for (int i = 0; i < param_count; ++i) {
		GLint value;

		// First, get the value.
		glGetProgramiv(m_program_id, prog_params[i].p, &value);
		assert(glGetError() == GL_NONE);

		// Stringify according to its type (as specified in the table)
		std::stringstream ss;

		switch (prog_params[i].type) {
		case GL_BOOL:
			ss << (value ? "GL_TRUE" : "GL_FALSE");
			break;
		case GL_INT:
			ss << (int)value;
			break;
		default:
			assert(false);
			break;
		}

		std::cout << prog_params[i].str << ": " << ss.str() << "\n";
	}

	std::cout << "attribute info:\n";
	std::cout << get_attribute_info();

	std::cout << "log:\n";
	std::cout << log();

	std::cout << "========| ***END*** Program::print_debug_info() |========\n";
}

GLint Program::get_program_param(GLenum param) const
{
	assert(m_program_id);
	assert(m_state != UNINITIALIZED);
	assert(is_program_param(param));

	GLint value = 0;
	glGetProgramiv(m_program_id, param, &value);
	assert(glGetError() == GL_NONE);

	return value;
}



std::string Program::get_attribute_info() const
{
	std::map<GLenum, std::string> type_map = {
		{GL_FLOAT, "GL_FLOAT"},
		{GL_FLOAT_VEC2, "GL_FLOAT_VEC2"},
		{GL_FLOAT_VEC3, "GL_FLOAT_VEC3"},
		{GL_FLOAT_VEC4, "GL_FLOAT_VEC4"},
		{GL_FLOAT_MAT2, "GL_FLOAT_MAT2"},
		{GL_FLOAT_MAT3, "GL_FLOAT_MAT3"},
		{GL_FLOAT_MAT4, "GL_FLOAT_MAT4"},
		{GL_FLOAT_MAT2x3, "GL_FLOAT_MAT2x3"},
		{GL_FLOAT_MAT2x4, "GL_FLOAT_MAT2x4"},
		{GL_FLOAT_MAT3x2, "GL_FLOAT_MAT2x4"},
		{GL_FLOAT_MAT3x4, "GL_FLOAT_MAT3x4"},
		{GL_FLOAT_MAT4x2, "GL_FLOAT_MAT4x2"},
		{GL_FLOAT_MAT4x3, "GL_FLOAT_MAT4x3"},
		{GL_INT, "GL_INT"},
		{GL_INT_VEC2, "GL_INT_VEC2"},
		{GL_INT_VEC3, "GL_INT_VEC3"},
		{GL_INT_VEC4, "GL_INT_VEC4"},
		{GL_UNSIGNED_INT, "GL_UNSIGNED_INT"},
		{GL_UNSIGNED_INT_VEC2, "GL_UNSIGNED_INT_VEC2"},
		{GL_UNSIGNED_INT_VEC3, "GL_UNSIGNED_INT_VEC3"},
		{GL_UNSIGNED_INT_VEC4, "GL_UNSIGNED_INT_VEC4"},
		{GL_DOUBLE, "GL_DOUBLE"},
		{GL_DOUBLE_VEC2, "GL_DOUBLE_VEC2"},
		{GL_DOUBLE_VEC3, "GL_DOUBLE_VEC3"},
		{GL_DOUBLE_VEC4, "GL_DOUBLE_VEC4"},
		{GL_DOUBLE_MAT2, "GL_DOUBLE_MAT2"},
		{GL_DOUBLE_MAT3, "GL_DOUBLE_MAT3"},
		{GL_DOUBLE_MAT4, "GL_DOUBLE_MAT4"},
		{GL_DOUBLE_MAT2x3, "GL_DOUBLE_MAT2x3"},
		{GL_DOUBLE_MAT2x4, "GL_DOUBLE_MAT2x4"},
		{GL_DOUBLE_MAT3x2, "GL_DOUBLE_MAT3x2"},
		{GL_DOUBLE_MAT3x4, "GL_DOUBLE_MAT3x4"},
		{GL_DOUBLE_MAT4x2, "GL_DOUBLE_MAT4x2"},
		{GL_DOUBLE_MAT4x3, "GL_DOUBLE_MAT4x3"}
	};

	std::stringstream info;

	GLint max_length = get_program_param(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
	GLint attrib_count = get_program_param(GL_ACTIVE_ATTRIBUTES);

	GLint size;
	GLenum type;

	for (int i = 0; i < attrib_count; ++i) {
		//std::string attrib_str(max_length, '\0');
		std::vector<GLchar> attrib_str(max_length, '\0');

		glGetActiveAttrib(m_program_id, i, max_length, NULL, &size, &type, attrib_str.data());
		assert(glGetError() == GL_NONE);

		auto it = type_map.find(type);
		assert(it != type_map.end());

		info << "\"" << std::string(attrib_str.data()) << "\": " << it->second << " (size = " << size << ")\n";
	}

	return info.str();
}

bool Program::is_program_param(GLenum param)
{
	return (param == GL_DELETE_STATUS)
		|| (param == GL_LINK_STATUS)
		|| (param == GL_VALIDATE_STATUS)
		|| (param == GL_INFO_LOG_LENGTH)
		|| (param == GL_ATTACHED_SHADERS)
		|| (param == GL_ACTIVE_ATTRIBUTES)
		|| (param == GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)
		|| (param == GL_ACTIVE_UNIFORMS)
		|| (param == GL_ACTIVE_UNIFORM_MAX_LENGTH);
}
