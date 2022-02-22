if (UNIX)
    set(CMAKE_CXX_FLAGS "-pthread")
endif (UNIX)

add_compile_definitions(LOG_LEVEL_ALL)

add_subdirectory(../CascadeLogging ../../build/CascadeLogging)

add_library(CascadeEngine src/entry_point.cpp)
target_link_libraries(CascadeEngine CascadeLogging)
target_include_directories(CascadeEngine PUBLIC include)