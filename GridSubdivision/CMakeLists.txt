cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build types." FORCE)
endif()

project(GridSubdivision)

include_directories(${PROJECT_SOURCE_DIR}/../API/GLAD/inc)
include_directories(${PROJECT_SOURCE_DIR}/../API/GLFW/inc)
include_directories(${PROJECT_SOURCE_DIR}/../API/GLM/inc)
include_directories(${PROJECT_SOURCE_DIR}/../API/TOOL/inc)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/API)
target_link_libraries(${PROJECT_NAME} PRIVATE API)