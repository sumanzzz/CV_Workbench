add_library(CV_Core STATIC
	ImageLoader.h ImageLoader.cpp
 "ImageOperations.h" "ImageOperations.cpp")
target_include_directories(CV_Core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CV_Core
    PUBLIC
    ${OpenCV_LIBS}
)