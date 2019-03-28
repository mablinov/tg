#include "Mesh.h"

Mesh::Mesh(WavefrontObj & obj)
{
	const WavefrontObj::MeshData &data = obj.data();

	vertex.reserve(data.vert.size());
	for (const auto &v : data.vert) {
		vertex.push_back(vec3f(v.x, v.y, v.z));
	}

	vertex.reserve(data.uv.size());
	for (const auto &v : data.uv) {
		uv.push_back(vec2f(v.x, v.y));
	}

	vertex.reserve(data.norm.size());
	for (const auto &v : data.norm) {
		normal.push_back(vec3f(v.x, v.y, v.z));
	}

	for (const auto &f : data.f) {
		if (f.p1.have.v) {
			vertex_tri.push_back(index_tri(f.p1.vertex, f.p2.vertex, f.p3.vertex));
		}
		if (f.p1.have.uv) {
			uv_tri.push_back(index_tri(f.p1.uv, f.p2.uv, f.p3.uv));
		}
		if (f.p1.have.n) {
			normal_tri.push_back(index_tri(f.p1.normal, f.p2.normal, f.p3.normal));
		}
	}
}

Mesh::Unpacked Mesh::unpack_to(GLenum mode)
{
	Unpacked unpacked;

	switch (mode) {
	case GL_TRIANGLES:
		unpacked = unpack_to_triangles();
		break;

	default:
		std::cerr << "Mesh::unpack_to(): Unsupported mode\n";
		break;
	}

	return unpacked;
}

Mesh::Unpacked Mesh::unpack_to_triangles()
{
	Unpacked unpacked;

	for (const auto &v : vertex_tri) {
		unpacked.vertex.push_back(vec3f(vertex[v.p1 - 1]));
		unpacked.vertex.push_back(vec3f(vertex[v.p2 - 1]));
		unpacked.vertex.push_back(vec3f(vertex[v.p3 - 1]));
	}

	for (const auto &u : uv_tri) {
		unpacked.uv.push_back(vec2f(uv[u.p1 - 1]));
		unpacked.uv.push_back(vec2f(uv[u.p2 - 1]));
		unpacked.uv.push_back(vec2f(uv[u.p3 - 1]));
	}

	for (const auto &n : normal_tri) {
		unpacked.normal.push_back(vec3f(normal[n.p1 - 1]));
		unpacked.normal.push_back(vec3f(normal[n.p2 - 1]));
		unpacked.normal.push_back(vec3f(normal[n.p3 - 1]));
	}

	return unpacked;
}

Mesh::index_tri::index_tri()
	: p1(0), p2(0), p3(0)
{
}

Mesh::index_tri::index_tri(size_t i1, size_t i2, size_t i3)
	: p1(i1), p2(i2), p3(i3)
{
}
