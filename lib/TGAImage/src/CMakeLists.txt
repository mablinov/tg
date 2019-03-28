add_library(TGAImage
	STATIC
	TGAImage.cpp
	${CMAKE_SOURCE_DIR}/lib/TGAImage/inc/TGAImage.h
)

target_include_directories(TGAImage
	PUBLIC
	${CMAKE_SOURCE_DIR}/lib/TGAImage/inc/
)
