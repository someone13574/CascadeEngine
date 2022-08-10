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
    src/Vulkan_Wrapper/device_wrapper.cpp
    src/Vulkan_Wrapper/surface_wrapper.cpp
    src/Vulkan_Wrapper/swapchain_wrapper.cpp
    src/Vulkan_Wrapper/pipeline_manager.cpp
    src/Vulkan_Wrapper/storage_manager.cpp
    src/Vulkan_Wrapper/storage_manager_resource_id.hpp
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
    src/object_manager.cpp
    src/window_information.hpp
    src/camera.cpp
    src/renderer.cpp)

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

if(UNIX)
    set(GLSL_COMMAND "/usr/bin/glslangValidator")
else()
    set(GLSL_COMMAND "$ENV{VK_SDK_PATH}/Bin/glslangValidator.exe")
endif(UNIX)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "src/Shaders/*.comp")

foreach(GLSL_FILE ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL_FILE} NAME)
    set(SPIRV_FILE "${PROJECT_BINARY_DIR}/src/Shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/src/Shaders/"
        COMMAND ${GLSL_COMMAND} -V ${GLSL_FILE} -o ${SPIRV_FILE}
        DEPENDS ${GLSL_FILE}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV_FILE})
endforeach(GLSL_FILE ${GLSL_SOURCE_FILES})

add_custom_target(Shaders
    DEPENDS ${SPIRV_BINARY_FILES})

add_dependencies(Cascade_Graphics Shaders)
add_custom_command(TARGET Cascade_Graphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:Cascade_Graphics>/src/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_BINARY_DIR}/src/Shaders"
    "$<TARGET_FILE_DIR:Cascade_Graphics>/src/Shaders")