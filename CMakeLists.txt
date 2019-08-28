cmake_minimum_required(VERSION 3.13.1)

project(raytracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

aux_source_directory(${CMAKE_SOURCE_DIR}/src PROJECT_SOURCES)
list(REMOVE_ITEM PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

set(LOCAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

add_compile_options(-Wall -Wextra -Wpedantic -g)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(out ${CMAKE_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCES})
target_include_directories(out PUBLIC ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${LOCAL_INCLUDE_DIRS})
target_link_libraries(out OpenGL::GL GLEW::GLEW glfw3 pthread X11 Xxf86vm Xrandr dl stdc++fs)

