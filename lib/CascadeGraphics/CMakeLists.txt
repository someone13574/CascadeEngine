cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../CascadeLogging ../../build/build/CascadeLogging)

if(WIN32)
    set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.204.1")
    find_package(Vulkan REQUIRED)
endif(WIN32)

add_library(CascadeGraphics STATIC
    src/VulkanWrapper/instance_wrapper.cpp
    src/VulkanWrapper/validation_layer_wrapper.cpp
    src/VulkanWrapper/physical_device_wrapper.cpp
    src/VulkanWrapper/queue_wrapper.cpp
    src/VulkanWrapper/device_wrapper.cpp
    src/VulkanWrapper/surface_wrapper.cpp
    src/VulkanWrapper/swapchain_wrapper.cpp
    src/VulkanWrapper/pipeline_wrapper.cpp
    src/VulkanWrapper/storage_manager.cpp
    src/debug_tools.cpp)

if(UNIX)
    target_include_directories(CascadeGraphics
        PUBLIC include)
else()
    target_include_directories(CascadeGraphics
        PUBLIC include
        PUBLIC "C:/VulkanSDK/1.3.204.1/Include")

    target_link_libraries(CascadeGraphics ${Vulkan_LIBRARIES})
endif(UNIX)

target_link_libraries(CascadeGraphics CascadeLogging)