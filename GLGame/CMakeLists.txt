cmake_minimum_required(VERSION 3.0.0)
project(GLGame VERSION 0.1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#build glfw from source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw-3.3.7)

#include header and source files
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")

#set(SOURCES
#    "src/main.cpp")

add_executable(GLGame ${SOURCES} src/graphics/glad.c include/physics/raycast.hpp include/graphics/objLoader.hpp include/graphics/model.hpp)
target_link_libraries(GLGame glfw)

#resource files
function(copy_resources)
    foreach(arg IN LISTS ARGN)
        configure_file(${arg} ${arg} COPYONLY)
    endforeach()
endfunction()
copy_resources( resources/shaders/shader.fs
                resources/shaders/shader.vs
                resources/shaders/texQuadShader.fs
                resources/shaders/texQuadShader.vs
                resources/shaders/debugDepthQuad.fs
                resources/shaders/debugDepthQuad.vs
                resources/awesomeface.png
                resources/container.jpg
                resources/fox.png
                resources/models/cube.obj)
