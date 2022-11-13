cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(../Cascade_Logging ../../build/build/Cascade_Logging)

if(WIN32)
    find_package(Vulkan REQUIRED)
endif(WIN32)

add_library(Cascade_Graphics STATIC
    src/Vulkan_Wrapper/instance_wrapper.cpp
    src/Vulkan_Wrapper/validation_layer_wrapper.cpp
    src/Vulkan_Wrapper/physical_device_wrapper.cpp
    src/Vulkan_Wrapper/queue_manager.cpp
    src/Vulkan_Wrapper/logical_device_wrapper.cpp
    src/Vulkan_Wrapper/surface_wrapper.cpp
    src/Vulkan_Wrapper/swapchain_wrapper.cpp
    src/Vulkan_Wrapper/pipeline_manager.cpp
    src/Vulkan_Wrapper/storage_manager.cpp
    src/Vulkan_Wrapper/identifier.hpp
    src/Vulkan_Wrapper/shader_manager.cpp
    src/Vulkan_Wrapper/command_buffer_manager.cpp
    src/Vulkan_Wrapper/descriptor_set_manager.cpp
    src/Vulkan_Wrapper/synchronization_manager.cpp
    src/Vulkan_Wrapper/debug_tools.cpp
    src/Data_Types/vector_2.hpp
    src/Data_Types/vector_3.hpp
    src/Data_Types/vector_4.hpp
    src/Data_Types/matrix_2x2.hpp
    src/Data_Types/matrix_3x3.hpp
    src/Data_Types/matrix_4x4.hpp
    src/object_manager.cpp
    src/window_information.hpp
    src/camera.cpp
    src/renderer.cpp
    src/Vulkan_Wrapper/vulkan_graphics.cpp)

if(UNIX)
    target_include_directories(Cascade_Graphics
        PUBLIC include)
else()
    target_include_directories(Cascade_Graphics
        PUBLIC include
        PUBLIC "$ENV{VK_SDK_PATH}/Include")

    target_link_libraries(Cascade_Graphics ${Vulkan_LIBRARIES})
endif(UNIX)

target_link_libraries(Cascade_Graphics Cascade_Logging)