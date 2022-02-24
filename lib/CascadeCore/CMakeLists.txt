if (UNIX)
    set(CMAKE_SHARED_LINKER_FLAGS "-Wall -Wextra -pthread -lxcb -g")
endif (UNIX)

add_compile_definitions(LOG_LEVEL_ALL)

add_subdirectory(../CascadeLogging ../../build/CascadeLogging)

add_library(CascadeCore SHARED
    src/event_manager.cpp
    src/window.cpp
    src/application.cpp)

target_include_directories(CascadeCore PUBLIC include)
target_link_libraries(CascadeCore CascadeLogging)