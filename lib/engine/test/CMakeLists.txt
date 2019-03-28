add_executable(test_Shader
	test_Shader.cpp
)

target_link_libraries(test_Shader
	PRIVATE GLUT::GLUT GLEW::GLEW
)

target_include_directories(test_Shader
	PRIVATE ${CMAKE_SOURCE_DIR}/lib/engine/inc
)

add_test(NAME test_Shader
	COMMAND test_Shader
)
