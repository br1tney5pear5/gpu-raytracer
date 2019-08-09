cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -Wpedantic -g)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_library(local STATIC Shader.cpp)

add_executable(out main.cpp)
target_include_directories(out PUBLIC ${OPENGL_INCLUDE_DIRS})
target_include_directories(out PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(out 
    local
    OpenGL::GL
    GLEW::GLEW
    glfw3
    pthread
    X11
    Xxf86vm
    Xrandr
    dl
    )

