cmake_minimum_required(VERSION 3.10)

project(Cascade VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-lxcb -lvulkan -pthread -g")
endif(UNIX)

add_compile_definitions(LOG_LEVEL_ALL CSD_ENABLE_DEBUG_LAYERS CSD_ENABLE_PRESENTATION)

add_subdirectory(lib/CascadeCore)

add_executable(CascadeApplication src/main.cpp)
target_link_libraries(CascadeApplication PUBLIC CascadeCore)