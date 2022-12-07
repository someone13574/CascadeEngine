# Define the minimum required version of cmake to build the logging library
cmake_minimum_required(VERSION 3.12...3.24)

# Create a static library for acorn logging
add_library(acorn_logging STATIC src/logger.cpp)

# Set 'include' as a include directory
target_include_directories(acorn_logging PUBLIC include)

# Enable C++17
target_compile_features(acorn_logging PRIVATE cxx_std_17)
set_target_properties(acorn_logging PROPERTIES CXX_EXTENSIONS OFF)

# Enable warnings
if (UNIX AND NOT APPLE)
    target_compile_options(acorn_logging PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
    target_compile_options(acorn_logging PRIVATE /W4 /WX)
endif()

# Link required thread library
find_package(Threads REQUIRED)
target_link_libraries(acorn_logging PRIVATE Threads::Threads)

# Set logging level
target_compile_definitions(acorn_logging PUBLIC LOG_LEVEL_ALL)