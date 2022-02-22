if (UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi")
endif (UNIX)

add_compile_definitions(LOG_LEVEL_ALL)

add_subdirectory(../CascadeLogging ../../build/CascadeLogging)

add_library(CascadeCore SHARED
    src/application.cpp 
    src/application.hpp
    src/window.cpp
    src/window.hpp)

target_include_directories(CascadeCore PUBLIC include)
target_link_libraries(CascadeCore CascadeLogging)