add_library(engine
	STATIC
	Shader.cpp
	Program.cpp
	${CMAKE_SOURCE_DIR}/lib/engine/inc/Shader.h
	${CMAKE_SOURCE_DIR}/lib/engine/inc/Program.h
)

target_include_directories(engine
	PUBLIC
	${CMAKE_SOURCE_DIR}/lib/engine/inc
)

target_link_libraries(engine
	PUBLIC GLUT::GLUT
	PUBLIC GLEW::GLEW
)
