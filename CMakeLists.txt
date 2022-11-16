# Define the minimum required version of cmake to build the test application
cmake_minimum_required(VERSION 3.12...3.24)

# Create the top level project
project(Cascade VERSION 0.7.0
                LANGUAGES CXX)

# Add an executable for a test application
add_executable(cascade_application src/main.cpp)

# Require C++ 17
target_compile_features(cascade_application PRIVATE cxx_std_17)
set_target_properties(cascade_application PROPERTIES CXX_EXTENSIONS OFF)

# Link core library
add_subdirectory(lib/Cascade_Core)
target_link_libraries(cascade_application PUBLIC cascade_core)