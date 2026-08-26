cmake_minimum_required(VERSION 3.20)
project(ProceduralVoxels)

# Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad SYSTEM PUBLIC external/glad/include)

add_library(glm INTERFACE)
target_include_directories(glm SYSTEM INTERFACE external/glm)

# GLFW build options - only core library
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

# Project Sources
file(GLOB_RECURSE SOURCES
  "src/*.cc"
  "include/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Configuration
target_include_directories(${PROJECT_NAME}
  PRIVATE include
  SYSTEM PRIVATE external/glfw/include
  SYSTEM PRIVATE external/stb
  SYSTEM PRIVATE external/json
)
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm)

# Compiler Warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)

  # /WX promotes warnings within stb_image to errors, failing the build
  set_source_files_properties(src/util/stb_image_impl.cc PROPERTIES
    COMPILE_OPTIONS "/wd4505"
  )
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

  # -Werror promotes warnings within stb_image to errors, failing the build
  set_source_files_properties(src/util/stb_image_impl.cc PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-function"
  )
endif()
