cmake_minimum_required(VERSION 3.10)

project(Cascade VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(UNIX)
    set(CMAKE_CXX_FLAGS "-lxcb -lvulkan -pthread -O0 -g")
endif(UNIX)

add_compile_definitions(LOG_LEVEL_ALL CSD_VULKAN_ENABLE_DEBUG_LAYERS)

add_subdirectory(lib/Cascade_Core)

add_executable(Cascade_Application src/main.cpp)
target_link_libraries(Cascade_Application PUBLIC Cascade_Core)