#include "WavefrontObj.h"

#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

WavefrontObj::vec4f::vec4f() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {
	
}

WavefrontObj::vec4f::vec4f(std::vector<float> &vec) : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {
	switch (vec.size()) {
	default: /* Fallthrough */
	case 4: /* Fallthrough */
		w = vec[3];
	case 3: /* Fallthrough */
		z = vec[2];
	case 2: /* Fallthrough */
		y = vec[1];
	case 1:
		x = vec[0];
		break;
	}
}

WavefrontObj::vec3f::vec3f()
	: x(0.0f), y(0.0f), z(0.0f)
{

}

WavefrontObj::vec3f::vec3f(std::vector<float> &vec)
	: x(0.0f), y(0.0f), z(0.0f)
{
	switch (vec.size()) {
	default: /* Fallthrough */
	case 3: /* Fallthrough */
		z = vec[2];
	case 2: /* Fallthrough */
		y = vec[1];
	case 1:
		x = vec[0];
		break;
	}
}

WavefrontObj::face_vertex_desc::face_vertex_desc()
	: have({ false, false, false }),
	vertex(0), normal(0), uv(0)
{
}

WavefrontObj::face_desc::face_desc()
	: p1(), p2(), p3()
{
}

WavefrontObj::MeshData::MeshData(
	std::vector<vec4f>& a, std::vector<vec3f>& b,
	std::vector<vec3f>& c, std::vector<face_desc>& d)
	: vert(a), norm(b), uv(c), f(d)
{
}

WavefrontObj::WavefrontObj()
{
}

WavefrontObj::WavefrontObj(std::string filename)
{
	std::ifstream ifs(filename, std::ios_base::binary);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open Wavefront .obj file: \"" << filename << "\"\n";
		return;
	}

	parse(ifs);
	ifs.close();
}

WavefrontObj::~WavefrontObj()
{
}

void WavefrontObj::parse(std::ifstream &ifs) {
	char buffer[MAX_LINE_LENGTH] = { 0 };

	if (ifs.eof()) {
		std::cerr << "Got empty Wavefront .obj file\n";
		return;
	}
	if (ifs.fail()) {
		std::cerr << "Failure to read Wavefront .obj file\n";
		return;
	}

	while (!ifs.eof()) {
		ifs.getline(buffer, MAX_LINE_LENGTH, '\n');

		if (ifs.bad()) {
			std::cerr << "Failed to read Wavefront .obj file during parse phase\n";
			return;
		}

		std::string line(buffer);
		parse_line(line);
	}
}

void WavefrontObj::parse_line(std::string &line) {
	enum DirectiveType {
		OBJECT_NAME,
		VERTEX_COORD,
		VERTEX_TEXTURE_COORD,
		VERTEX_NORMAL_COORD,
		FACE,

		DIRECTIVE_COUNT
	};

	static const struct directive_map {
		DirectiveType type;
		const char *str;
	} directive[] = {
		{OBJECT_NAME, "o"},
		{VERTEX_COORD, "v"},
		{VERTEX_TEXTURE_COORD, "vt"},
		{VERTEX_NORMAL_COORD, "vn"},
		{FACE, "f"}
	};

	// Check if we have a hash anywhere, and if we do, discard the string including it
	// and everything after.
	size_t hashpos = line.find_first_of('#');
	if (hashpos != std::string::npos) {
		line.erase(hashpos);
	}

	// Check if there is even anything to parse
	if (line.size() == 0)
		return;

	std::stringstream ss(line);
	std::string direct;
	ss >> direct;

	int found = 0;
	int idx;

	for (idx = 0; idx < DIRECTIVE_COUNT; ++idx) {
		if (direct.compare(directive[idx].str) == 0) {
			found = 1;
			break;
		}
	}

	if (!found) {
		std::cerr << "Unknown directive \"" << direct << "\"\n";
		return;
	}

	DirectiveType type = directive[idx].type;
	switch (type) {
	case OBJECT_NAME:
		process_object_name(ss);
		break;

	case VERTEX_COORD:
		process_vertex_coord(ss);
		break;

	case VERTEX_TEXTURE_COORD:
		process_uv_coord(ss);
		break;

	case VERTEX_NORMAL_COORD:
		process_vertex_normal_coord(ss);
		break;

	case FACE:
		process_face(ss);
		break;

	default:
		std::cerr << "Unknown type caught in WavefrontObj::parse_line()\n";
		break;
	}
}

void WavefrontObj::process_object_name(std::stringstream & ss)
{
	std::string name;
	ss >> name;
	object_name = name;
}

void WavefrontObj::process_vertex_coord(std::stringstream & ss)
{
	std::vector<float> values;

	while (ss.good()) {
		float v;
		ss >> v;
		values.push_back(v);
	}

	if (values.size() == 0) {
		return;
	}

	vec4f vec(values);
	vert.push_back(vec);
}

void WavefrontObj::process_vertex_normal_coord(std::stringstream & ss)
{
	std::vector<float> values;

	while (ss.good()) {
		float v;
		ss >> v;
		values.push_back(v);
	}

	if (values.size() == 0) {
		return;
	}

	vec3f vec(values);
	norm.push_back(vec);
}

void WavefrontObj::process_uv_coord(std::stringstream & ss)
{
	std::vector<float> values;

	while (ss.good()) {
		float v;
		ss >> v;
		values.push_back(v);
	}

	if (values.size() == 0) {
		return;
	}

	vec3f vec(values);
	uv.push_back(vec);
}

void WavefrontObj::process_face(std::stringstream & ss)
{
	std::vector<std::string> face_vert_list;
	std::string arg;
	face_desc f_desc;

	size_t arg_cnt = 0;

	while (ss.good()) {
		ss >> arg;
		face_vertex_desc desc = parse_face_arg(arg);

		switch (arg_cnt) {
		case 0: f_desc.p1 = desc; break;
		case 1: f_desc.p2 = desc; break;
		case 2: f_desc.p3 = desc; break;
		default:
			assert(false);
		}

		arg_cnt++;
	}

	assert(arg_cnt == 3);

	f.push_back(f_desc);
}

WavefrontObj::face_vertex_desc WavefrontObj::parse_one_param(std::string & arg)
{
	face_vertex_desc desc;

	// If we're here, then by definition we only have the vertex parameter.
	desc.have = { true, false, false };

	std::stringstream ss(arg);
	size_t vertex_idx;

	ss >> vertex_idx;

	desc.vertex = vertex_idx;
	return desc;
}

WavefrontObj::face_vertex_desc WavefrontObj::parse_two_param(std::string & arg)
{
	face_vertex_desc desc;

	// If we're here, then we have a vertex and texture coord (maybe).
	desc.have = { true, true, false };

	std::string::iterator delim = std::find(arg.begin(), arg.end(), '/');
	std::istringstream ss;

	std::string left(arg.begin(), delim);
	if (left.size()) {
		desc.have.v = true;

		ss.str(left);
		ss >> desc.vertex;
		ss.clear();
	}

	std::string right(delim + 1, arg.end());
	if (right.size()) {
		desc.have.uv = true;

		ss.str(right);
		ss >> desc.uv;
		ss.clear();
	}

	return desc;
}

WavefrontObj::face_vertex_desc WavefrontObj::parse_three_param(std::string & arg)
{
	face_vertex_desc desc;

	// If we're here, then we MIGHT have SOME COMBINATION
	// of a vertex, texture, and normal coord.

	std::string::iterator delim1 = std::find(arg.begin(), arg.end(), '/');
	std::string::iterator delim2 = std::find(delim1 + 1, arg.end(), '/');

	std::string v_str(arg.begin(), delim1);
	std::string uv_str(delim1 + 1, delim2);
	std::string n_str(delim2 + 1, arg.end());

	std::istringstream ss;

	if (v_str.size()) {
		desc.have.v = true;

		ss.str(v_str);
		ss >> desc.vertex;
		ss.clear();
	}

	if (uv_str.size()) {
		desc.have.uv = true;

		ss.str(uv_str);
		ss >> desc.uv;
		ss.clear();
	}
	if (n_str.size()) {
		desc.have.n = true;

		ss.str(n_str);
		ss >> desc.normal;
		ss.clear();
	}

	return desc;
}

WavefrontObj::MeshData WavefrontObj::data()
{
	MeshData ret(vert, norm, uv, f);
	return ret;
}

WavefrontObj::face_vertex_desc WavefrontObj::parse_face_arg(std::string &arg)
{
	face_vertex_desc desc;
	std::vector<size_t> delim_pos;

	for (size_t i = 0; i < arg.size(); ++i) {
		if (arg[i] == '/') {
			delim_pos.push_back(i);
		}
	}

	switch (delim_pos.size()) {
	case 0:
		desc = parse_one_param(arg);
		break;

	case 1:
		desc = parse_two_param(arg);
		break;

	case 2:
		desc = parse_three_param(arg);
		break;

	default:
		assert(false);
		break;
	}

	return desc;
}
