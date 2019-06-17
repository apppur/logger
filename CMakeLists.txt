project (spdlog_test)
cmake_minimum_required(VERSION 3.4)

# macro
add_definitions(-D _DEBUG)

set(CMAKE_CXX_STANDARD 14)

add_library(spdlog INTERFACE)
target_include_directories(
    spdlog
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../spdlog/include>"
)

message(STATUS "**********************************************")
message(STATUS "CMAKE_SOURCE_DIR->" ${CMAKE_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR->" ${PROJECT_SOURCE_DIR})
message(STATUS "**********************************************")

find_package(Threads)

add_executable (spdlog_test test.cpp)
target_link_libraries(spdlog_test spdlog ${CMAKE_THREAD_LIBS_INIT})
