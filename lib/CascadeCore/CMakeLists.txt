cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../CascadeGraphics ../../build/build/CascadeGraphics)

add_library(CascadeCore STATIC
    src/event_manager.cpp
    src/renderer.cpp
    src/window.cpp
    src/application.cpp)

target_include_directories(CascadeCore PUBLIC include)

target_link_libraries(CascadeCore CascadeGraphics)