cmake_minimum_required(VERSION 3.16)
project(LearnGL LANGUAGES C CXX)

macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

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
include_directories(glm)

# Subprojects
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(glm)

# Main executable
add_executable(LearnGL main.cpp shader.cpp shader.h "window.h"  "resource_manager.cpp" "camera.h" "mesh.h" "resource_manager.h" "tuplehash.h" "model.h" "mesh.cpp" "model.cpp" "utils.h" "material.h" "material.cpp")

# Linking
target_link_libraries(LearnGL ${OpenGL_LIB_NAMES} glad glfw)

add_custom_command(TARGET LearnGL PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/textures/ $<TARGET_FILE_DIR:LearnGL>/textures
                   COMMENT "Copied textures to build dir.")

add_custom_command(TARGET LearnGL POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:LearnGL>/shaders
                   COMMENT "Copied shaders to build dir.")

add_custom_command(TARGET LearnGL POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/meshes/ $<TARGET_FILE_DIR:LearnGL>/meshes
                   COMMENT "Copied meshes to build dir.")