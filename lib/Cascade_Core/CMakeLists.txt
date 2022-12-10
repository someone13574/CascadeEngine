# Define the minimum required version of cmake to build the core library
cmake_minimum_required(VERSION 3.16...3.24)

# Create a static library for cascade core
add_library(cascade_core STATIC src/application_factory.cpp
                                src/application.cpp
                                src/thread_manager.cpp
                                src/thread.cpp
                                src/entry.cpp
                                src/window.cpp
                                src/window_factory.cpp
                                src/xcb_window.cpp
                                src/xcb_window_factory.cpp
                                src/win32_window.cpp
                                src/win32_window_factory.cpp)

# Set 'include' as a include directory
target_include_directories(cascade_core PUBLIC include)

# Enable C++17
target_compile_features(cascade_core PRIVATE cxx_std_17)
set_target_properties(cascade_core PROPERTIES CXX_EXTENSIONS OFF)

# Enable warnings
if (UNIX AND NOT APPLE)
    target_compile_options(cascade_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
    target_compile_options(cascade_core PRIVATE /W4 /WX)
endif()

# Link required thread library
find_package(Threads REQUIRED)
target_link_libraries(cascade_core PRIVATE Threads::Threads)

# Link graphics library
add_subdirectory(../Cascade_Graphics ../../build/lib/Cascade_Graphics)
target_link_libraries(cascade_core PUBLIC cascade_graphics)

# Link XCB
if (UNIX AND NOT APPLE)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

    find_package(XCB REQUIRED)
    target_link_libraries(cascade_core PRIVATE ${XCB_LIBRARIES})
endif()