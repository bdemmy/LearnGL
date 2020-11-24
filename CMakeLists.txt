cmake_minimum_required(VERSION 3.16)
project(LearnGL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

include_directories(glfw/include ${OpenGL_INCLUDE_DIRS} glad/include)

add_subdirectory(glad/)

add_executable(LearnGL main.cpp shader.cpp shader.h)
target_link_libraries(LearnGL ${OpenGL_LIB_NAMES} glad glfw3)