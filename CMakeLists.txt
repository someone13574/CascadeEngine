cmake_minimum_required(VERSION 3.10)

project(Cascade VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(LOG_LEVEL_ALL CSD_VULKAN_ENABLE_DEBUG_LAYERS)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(XCB REQUIRED xcb)
pkg_check_modules(VULKAN REQUIRED vulkan)
pkg_check_modules(SHADERC REQUIRED shaderc)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_subdirectory(lib/Cascade_Core)

include_directories(${XCB_INCLUDE_DIRS} ${VULKAN_INCLUDE_DIRS} ${SHADERC_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} Cascade_Core ${XCB_LIBRARIES} ${VULKAN_LIBRARIES} ${SHADERC_LIBRARIES} Threads::Threads)