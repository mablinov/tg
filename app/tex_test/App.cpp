#include "App.h"

#include "vec2f.h"
#include "vec3f.h"
#include "vec4f.h"
#include "TGAImage.h"
#include "WavefrontObj.h"
#include "Mesh.h"

#include "GL/glew.h"
#include "GL/freeglut.h"

App::App() {

}

void App::init()
{
#if 0
	init_array();
	init_tex();
#endif
	init_tex();
	init_program();
}

size_t App::get_triangle_count() const
{
	return triangle_count;
}

std::vector<vec4f> App::conv_tga_to_gltexture(const TGAImage & image) const
{
	std::vector<vec4f> data;
	data.resize(image.width() * image.height(), { 0.0f, 0.0f, 0.0f, 0.0f });

	for (int y = 0; y < image.height(); ++y) {
		for (int x = 0; x < image.width(); ++x) {
			TGAImage::rgba pixel = image.getPixel(x, y);
			vec4f float_pixel;

			float_pixel.x = pixel.r / 255.0f;
			float_pixel.y = pixel.g / 255.0f;
			float_pixel.z = pixel.b / 255.0f;
			float_pixel.w = pixel.a / 255.0f;

			data.push_back(float_pixel);
		}
	}

	return data;
}

void App::init_array()
{
	static const vec3f vertex[3] = {
	vec3f(0.5f, 0.5f, 0.0f),
	vec3f(0.5f, -0.5f, 0.0f),
	vec3f(-0.5f, -0.5f, 0.0f),
#if 0
	vec3f(0.5f, 0.5f, 0.0f),
	vec3f(-0.5f, 0.5f, 0.0f),
	vec3f(-0.5f, -0.5f, 0.0f),
#endif
	};

	static const vec3f uv[6] = {
		vec3f(1.0f, 1.0f, 0.0f),
		vec3f(1.0f, 0.0f, 0.0f),
		vec3f(0.0f, 0.0f, 0.0f),

		vec3f(1.0f, 1.0f, 0.0f),
		vec3f(0.0f, 1.0f, 0.0f),
		vec3f(0.0f, 0.0f, 0.0f),
	};

	static const vec3f normal[6] = {
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),

		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f)
	};

	glGenBuffers(1, &buffer.vertex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3f), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &buffer.uv);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.uv);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3f), uv, GL_STATIC_DRAW);

	glGenBuffers(1, &buffer.normal);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.normal);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3f), normal, GL_STATIC_DRAW);
}

void App::init_tex()
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	TGAImage image("res/metal1.tga");
	const TGAImage::rgba *data = image.getPixelData().data();

	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		image.width(),
		image.height(),
		0,
		GL_BGRA, // B and R channels are flipped in .tga formats
		GL_UNSIGNED_BYTE,
		(void *)data
	);

	// Must load texture before generating mipmaps.
	glGenerateMipmap(GL_TEXTURE_2D);
}

void App::init_program()
{
	static const char *vs_source =
		"#version 110\n"
		"attribute vec3 vertex;\n"
		"attribute vec2 uv;\n"
		"varying vec2 frag_uv;\n"
		""
		"void main() {\n"
		" gl_Position = vec4(vertex, 1.0);\n"
		" frag_uv = uv;\n"
		"}\n"
		;

	static const char *fs_source =
		"#version 110\n"
		""
		""
		"varying vec2 frag_uv;\n"
		""
		"uniform sampler2D tex;\n"
		""
		"void main() {\n"
		" gl_FragColor = texture2D(tex, frag_uv);\n"
		"}\n"
		;

	vs = new Shader(std::string(vs_source), GL_VERTEX_SHADER);
	fs = new Shader(std::string(fs_source), GL_FRAGMENT_SHADER);

	p = new Program(*vs, *fs);

	vs->print_debug_info();
	fs->print_debug_info();
	p->print_debug_info();
	p->use();

	GLint vertex_id = glGetAttribLocation(p->id(), "vertex");
	GLint uv_id = glGetAttribLocation(p->id(), "uv");

	WavefrontObj mesh("res/coollogo.obj");
	Mesh temp(mesh);
	Mesh::Unpacked unpacked = temp.unpack_to_triangles();

#if 0
	static const vec3f vertex[6] = {
		vec3f(0.5f, 0.5f, 0.0f),
		vec3f(0.5f, -0.5f, 0.0f),
		vec3f(-0.5f, -0.5f, 0.0f),

		vec3f(0.5f, 0.5f, 0.0f),
		vec3f(-0.5f, 0.5f, 0.0f),
		vec3f(-0.5f, -0.5f, 0.0f),
	};

	static const vec2f uv[6] = {
		vec2f(1.0f, 1.0f),
		vec2f(1.0f, 0.0f),
		vec2f(0.0f, 0.0f),

		vec2f(1.0f, 1.0f),
		vec2f(0.0f, 1.0f),
		vec2f(0.0f, 0.0f),
	};

	static const vec3f normal[6] = {
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),

		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f),
		vec3f(0.0f, 0.0f, 1.0f)
	};
#elseif 0
	const vec3f *vertex = unpacked.vertex.data();
	const vec2f *uv = unpacked.uv.data();
	const vec3f *normal = unpacked.normal.data();
#endif

	triangle_count = temp.vertex_tri.size() * 3;
#if 0
	glGenBuffers(1, &buffer.vertex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3f), (void *)vertex, GL_STATIC_DRAW);
#endif

	glGenBuffers(1, &buffer.vertex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
	glBufferData(GL_ARRAY_BUFFER, unpacked.vertex.size() * sizeof(vec3f), (void *)unpacked.vertex.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertex_id);
	glVertexAttribPointer(vertex_id, 3, GL_FLOAT, GL_FALSE, 0, 0);


	assert(glGetError() == GL_NONE);
#if 1
	glGenBuffers(1, &buffer.uv);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.uv);
	glBufferData(GL_ARRAY_BUFFER, unpacked.uv.size() * sizeof(vec3f), (void *)unpacked.uv.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(uv_id, 2, GL_FLOAT, GL_TRUE, 0, 0);
	glEnableVertexAttribArray(uv_id);
#endif
	assert(glGetError() == GL_NONE);

#if 0
	glGenBuffers(1, &buffer.normal);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.normal);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(vec3f), normal, GL_STATIC_DRAW);
#endif

	return;
}
