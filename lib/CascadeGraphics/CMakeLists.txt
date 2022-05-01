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
    src/VulkanWrapper/shader_manager.cpp
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

if(UNIX)
    set(GLSL_COMMAND "/usr/bin/glslangValidator")
else()
    set(GLSL_COMMAND "$ENV{VULKAN_SDK}/bin/glslangValidator.exe")
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

add_dependencies(CascadeGraphics Shaders)
add_custom_command(TARGET CascadeGraphics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:CascadeGraphics>/src/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_BINARY_DIR}/src/Shaders"
    "$<TARGET_FILE_DIR:CascadeGraphics>/src/Shaders")