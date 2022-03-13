cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../CascadeLogging ../../build/build/CascadeLogging)
add_library(CascadeGraphics STATIC src/cascade_graphics.cpp)

target_include_directories(CascadeGraphics PUBLIC include)
target_link_libraries(CascadeGraphics CascadeLogging)