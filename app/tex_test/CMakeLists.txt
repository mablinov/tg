add_executable(tex_test
	App.cpp
	App.h
	mat4.cpp
	mat4.h
	Mesh.cpp
	Mesh.h
	textest.cpp
	vec2f.h
	vec3f.h
	vec4f.h
)

target_link_libraries(tex_test
	PRIVATE engine TGAImage WavefrontObj
        GLEW::GLEW GLUT::GLUT OpenGL::GL
)

add_custom_command(TARGET tex_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/app/tex_test/res $<TARGET_FILE_DIR:tex_test>/res
)

# If we're using Visual Studio, then we also want to copy the res/ directory
# into the same location as the .sln file, so that when debugged from
# within visual studio, the program can still find the files.
if(WIN32)
add_custom_command(TARGET tex_test POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/app/tex_test/res $<TARGET_FILE_DIR:tex_test>/../res
)
endif()
