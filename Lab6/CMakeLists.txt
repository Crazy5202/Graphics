cmake_minimum_required(VERSION 3.10)
project(fifth_lab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/imgui/include)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/imgui/src/*.cpp)
add_library(lab_lib ${SOURCES})

add_executable(main main_4.cpp)

target_link_libraries(main lab_lib sfml-graphics sfml-window sfml-system GL GLU GLEW)