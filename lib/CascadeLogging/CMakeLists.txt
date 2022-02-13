set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_library(CascadeLogging
    src/log_stream.cpp
    src/logger.cpp
    src/log_stream.hpp
    src/logger.hpp
)
target_include_directories(CascadeLogging PUBLIC include)