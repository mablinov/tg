#include <iostream>

#include "GL/glew.h"
#include "GL/glut.h"

#include "App.h"
#include "mat4.h"

static App app;

void display();

void init() {
	glutDisplayFunc(display);
	app.init();
}

void display() {
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	size_t count = app.get_triangle_count();
	glDrawArrays(GL_TRIANGLES, 0, count);
	GLenum err = glGetError();
	if (err != GL_NONE) {
		std::cout << "Got error right on draw\n";
		std::terminate();
	}

	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(200, 200);
	glutInitDisplayMode(GLUT_RGBA);

	int win = glutCreateWindow("Textures");

	GLenum error = glewInit();
	if (error != GL_NONE) {
		std::cerr << "Failed to initialize glew\n";
		return -1;
	}

	mat4 m = mat4::identity();
	vec4 &c0 = m[0];
	vec4 &c1 = m[1];
	vec4 &c2 = m[2];
	vec4 &c3 = m[3];

	c1.x = 5.0f;
	c3.w = 8.5f;

	init();
	glutMainLoop();

	glutDestroyWindow(win);
	return 0;
}

