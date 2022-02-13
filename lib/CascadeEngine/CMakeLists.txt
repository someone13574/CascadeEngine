set(CMAKE_CXX_FLAGS "-pthread --define-macro LOG_LEVEL_ALL")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_subdirectory(../CascadeLogging ../../build/CascadeLogging)

add_library(CascadeEngine src/entry_point.cpp)
target_link_libraries(CascadeEngine CascadeLogging)
target_include_directories(CascadeEngine PUBLIC include)