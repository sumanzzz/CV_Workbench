cmake_minimum_required(VERSION 3.20)

project(CV_Workbench
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OpenCV_DIR "D:/opencv/build")
set(CMAKE_PREFIX_PATH "D:/Qt/6.11.2/msvc2022_64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include directories: ${OpenCV_INCLUDE_DIRS}")


add_executable(CV_Workbench
    src/main.cpp
)

target_include_directories(CV_Workbench
    PRIVATE ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(CV_Workbench
    PRIVATE ${OpenCV_LIBS}
)
target_link_libraries(CV_Workbench
    PRIVATE
    ${OpenCV_LIBS}
    Qt6::Widgets
)