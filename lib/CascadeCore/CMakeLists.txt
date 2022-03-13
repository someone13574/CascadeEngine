cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../CascadeGraphics ../../build/build/CascadeGraphics)

if(WIN32)
    set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.204.1")
    find_package(Vulkan REQUIRED)
endif(WIN32)

add_library(CascadeCore STATIC
    src/event_manager.cpp
    src/renderer.cpp
    src/window.cpp
    src/application.cpp)

if(UNIX)
    target_include_directories(CascadeCore
        PUBLIC include)
else()
    target_include_directories(CascadeCore
        PUBLIC include
        PUBLIC "C:/VulkanSDK/1.3.204.1/Include")

    target_link_libraries(CascadeCore ${Vulkan_LIBRARIES})
endif(UNIX)

target_link_libraries(CascadeCore CascadeGraphics)