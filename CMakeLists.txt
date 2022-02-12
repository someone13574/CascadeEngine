cmake_minimum_required(VERSION 3.10)

project(Cascade VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib/CascadeEngine)

add_executable(CascadeApplication main.cpp)
target_link_libraries(CascadeApplication PUBLIC CascadeEngine)