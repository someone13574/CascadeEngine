cmake_minimum_required(VERSION 3.10)

if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread")
endif (UNIX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(Cascade_Logging STATIC
    src/log_stream.cpp
    src/logger.cpp)

target_include_directories(Cascade_Logging PUBLIC include)