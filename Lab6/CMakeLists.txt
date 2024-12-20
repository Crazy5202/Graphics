cmake_minimum_required(VERSION 3.10)
project(sixth_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui/include ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/json)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/*.cpp)
add_library(lab_lib ${SOURCES})

add_executable(main main.cpp)

target_link_libraries(main lab_lib GL GLU GLEW glfw)