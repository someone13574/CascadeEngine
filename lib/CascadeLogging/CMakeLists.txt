if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread")
endif (UNIX)

add_library(CascadeLogging SHARED
    src/log_stream.cpp
    src/logger.cpp
    src/log_stream.hpp
    src/logger.hpp
)
target_include_directories(CascadeLogging PUBLIC include)