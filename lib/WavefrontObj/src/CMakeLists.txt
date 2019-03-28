add_library(WavefrontObj
	STATIC
	WavefrontObj.cpp
	${CMAKE_SOURCE_DIR}/lib/WavefrontObj/inc/WavefrontObj.h
)

target_include_directories(WavefrontObj
	PUBLIC
	${CMAKE_SOURCE_DIR}/lib/WavefrontObj/inc
)
