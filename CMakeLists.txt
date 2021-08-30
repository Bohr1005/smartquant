cmake_minimum_required(VERSION 3.11)

project(SmartQuant
        VERSION 1.0.0.0
        LANGUAGES CXX)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_compile_options(-std=c++17)

message(STATUS "CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE   = ${CMAKE_BUILD_TYPE}")

include_directories(${PROJECT_SOURCE_DIR}/3rd/spdlog/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/)
include_directories(${PROJECT_SOURCE_DIR}/smartquant)

add_subdirectory(${PROJECT_SOURCE_DIR}/smartquant)



