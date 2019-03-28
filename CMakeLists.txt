cmake_minimum_required(VERSION 3.2)
project(TankGame)

#Suppress Policy CMP0072
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL
        REQUIRED
)

find_package(GLUT
	REQUIRED
)

find_package(GLEW
	REQUIRED
)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(app)
