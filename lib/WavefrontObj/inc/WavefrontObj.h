#pragma once

#include <iostream>
#include <string>
#include <vector>

class WavefrontObj {
public:
	static const size_t MAX_LINE_LENGTH = 256;

	struct vec4f {
		float x, y, z, w;

		vec4f();
		vec4f(std::vector<float> &vec);
	};

	struct vec3f {
		float x, y, z;

		vec3f();
		vec3f(std::vector<float> &vec);
	};

	struct face_vertex_desc {
		struct {
			bool v, uv, n;
		} have;

		size_t vertex;
		size_t normal;
		size_t uv;

		face_vertex_desc();
	};

	struct face_desc {
		face_vertex_desc p1, p2, p3;

		face_desc();
	};

	struct MeshData {
		std::vector<vec4f> &vert;
		std::vector<vec3f> &norm;
		std::vector<vec3f> &uv;
		std::vector<face_desc> &f;

		MeshData(std::vector<vec4f> &a, std::vector<vec3f> &b,
			std::vector<vec3f> &c, std::vector<face_desc> &d);
	};


private:
	std::string object_name;

	std::vector<vec4f> vert;
	std::vector<vec3f> norm;
	std::vector<vec3f> uv;
	std::vector<face_desc> f;


public:
	WavefrontObj();
	explicit WavefrontObj(std::string filename);
	~WavefrontObj();

	MeshData data();

	void parse(std::ifstream &ifs);
	void parse_line(std::string &line);

	void process_object_name(std::stringstream &ss);
	void process_vertex_coord(std::stringstream &ss);
	void process_vertex_normal_coord(std::stringstream &ss);
	void process_uv_coord(std::stringstream &ss);
	void process_face(std::stringstream &ss);

	face_vertex_desc parse_face_arg(std::string &arg);
	face_vertex_desc parse_one_param(std::string &arg);
	face_vertex_desc parse_two_param(std::string & arg);
	face_vertex_desc parse_three_param(std::string & arg);

};