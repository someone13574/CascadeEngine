# Define the minimum required version of cmake to build the graphics library
cmake_minimum_required(VERSION 3.16...3.24)

# Creaet a static library for cascade graphics
add_library(cascade_graphics STATIC src/window_info.cpp
                                    src/win32_window_info.cpp
                                    src/xcb_window_info.cpp
                                    src/graphics_factory.hpp
                                    src/graphics.cpp
                                    src/renderer.cpp
                                    src/graphics_factory.cpp
                                    src/vulkan_graphics_factory.cpp
                                    src/vulkan_graphics.cpp
                                    src/vulkan_renderer.cpp
                                    src/Vulkan_Backend/instance.cpp
                                    src/Vulkan_Backend/instance_builder.cpp
                                    src/Vulkan_Backend/queue_selector.cpp
                                    src/Vulkan_Backend/physical_device.cpp
                                    src/Vulkan_Backend/physical_device_selector.cpp
                                    src/Vulkan_Backend/device.cpp
                                    src/Vulkan_Backend/surface.cpp
                                    src/Vulkan_Backend/win32_surface.cpp
                                    src/Vulkan_Backend/xcb_surface.cpp
                                    src/Vulkan_Backend/vkresult_translator.cpp
                                    src/Vulkan_Backend/vulkan_header.hpp)

# Set 'include' as a include directory
target_include_directories(cascade_graphics PUBLIC include)

# Enable C++17
target_compile_features(cascade_graphics PRIVATE cxx_std_17)
set_target_properties(cascade_graphics PROPERTIES CXX_EXTENSIONS OFF)

# Enable warnings
if (UNIX AND NOT APPLE)
    target_compile_options(cascade_graphics PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
    target_compile_options(cascade_graphics PRIVATE /W4 /WX)
endif()

# Link logging library
add_subdirectory(../Acorn_Logging ../../build/lib/Acorn_Logging)
target_link_libraries(cascade_graphics PUBLIC acorn_logging)

# Link Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(cascade_graphics PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(cascade_graphics PUBLIC ${Vulkan_INCLUDE_DIRS})