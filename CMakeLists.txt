cmake_minimum_required(VERSION 3.2)
project(TankGame)

find_package(GLUT
	REQUIRED
)

find_package(GLEW
	REQUIRED
)

add_subdirectory(src)
add_subdirectory(lib)
add_subdirectory(app)
