cmake_minimum_required(VERSION 3.20)

project(ProceduralVoxels)

# Glad library
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

# GLFW
add_subdirectory(external/glfw)

add_executable(ProceduralVoxels src/main.cpp)
target_link_libraries(ProceduralVoxels PRIVATE glad glfw)

