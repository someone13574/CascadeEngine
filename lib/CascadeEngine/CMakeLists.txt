add_subdirectory(../CascadeLogging ../../build/CascadeLogging)

add_library(CascadeEngine src/entry_point.cpp)
target_link_libraries(CascadeEngine CascadeLogging)
target_include_directories(CascadeEngine PUBLIC include)