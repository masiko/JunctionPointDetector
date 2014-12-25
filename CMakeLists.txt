cmake_minimum_required(VERSION 2.8)

project(DowmScale)

set(CMAKE_CXX_FLAGS_RELEASE)
find_package(OpenCV REQUIRED)
add_executable(DownScale test_main.cpp DownScale.cpp JunctionPointDetector.cpp lsd.c)
target_link_libraries(DownScale ${OpenCV_LIBS})
