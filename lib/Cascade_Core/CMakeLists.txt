cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../Cascade_Graphics ../../build/build/Cascade_Graphics)

add_library(Cascade_Core STATIC
    src/renderer.cpp
    src/window.cpp
    src/application.cpp
    src/event_manager.cpp)

target_include_directories(Cascade_Core PUBLIC include)

target_link_libraries(Cascade_Core Cascade_Graphics)