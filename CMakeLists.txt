cmake_minimum_required(VERSION 3.16)
project(LearnGL LANGUAGES C CXX)

# If using visual studio
if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        # Use latest c++ standard
        add_compile_options("/std:c++latest")
    endif()
else()
    # Otherwise use 17
    set(CMAKE_CXX_STANDARD 17)
endif()

# Make sure we can find opengl
find_package(OpenGL REQUIRED)

# Set include directories
include_directories(glfw/include)
include_directories(${OpenGL_INCLUDE_DIRS})
include_directories(glad/include)

# Subprojects
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_subdirectory(glad)

# Main executable
add_executable(LearnGL main.cpp shader.cpp shader.h)

# Linking
target_link_libraries(LearnGL ${OpenGL_LIB_NAMES} glad glfw)